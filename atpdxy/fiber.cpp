#include "fiber.h"
#include <atomic>
#include "config.h"
#include "log.h"
#include "macro.h"
#include "scheduler.h"
namespace atpdxy{
static Logger::ptr g_logger = ATPDXY_LOG_NAME("system");
// 协程id
static std::atomic<uint64_t> s_fiber_id {0};
// 协程总数
static std::atomic<uint64_t> s_fiber_count {0};
// 当前线程的协程指针
static thread_local Fiber* t_fiber = nullptr;
// 智能指针表示当前协程
static thread_local Fiber::ptr t_threadFiber = nullptr;
// 定义栈大小
static ConfigVar<uint32_t>::ptr g_fiber_stack_size = 
    Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");

class  MallocStackAllocator{
public:
    static void* Alloc(size_t size){
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size){
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

// 将线程的上下文赋值给这个main协程
Fiber::Fiber(){
    m_state = EXEC;
    SetThis(this);
    
    // 获取当前上下文并保存到main_fiber的m_ctx中
    if(getcontext(&m_ctx)){
        ATPDXY_ASSERT2(false, "getcontext");
    }
    ++s_fiber_count;
    ATPDXY_LOG_DEBUG(g_logger) << " Fiber::Fiber main";
}

void Fiber::SetThis(Fiber* f){
    t_fiber = f;
}

// 构造，包含协程执行的函数和栈空间
Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller)
    :m_id(++s_fiber_id), m_cb(cb){
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();
    // 栈的生成
    m_stack = StackAllocator::Alloc(m_stacksize);
    if(getcontext(&m_ctx)){
        ATPDXY_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    // 制作新的上下文
    if(!use_caller){
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    }else{
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }
    
    ATPDXY_LOG_DEBUG(g_logger) << " Fiber::Fiber id=" << m_id;
}

Fiber::~Fiber(){
    --s_fiber_count;
    if(m_stack){
        ATPDXY_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT);
        StackAllocator::Dealloc(m_stack, m_stacksize);
    }else{
        // 没有栈说明是主协程，主协程一直运行
        ATPDXY_ASSERT(!m_cb);
        ATPDXY_ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if(cur == this){
            SetThis(nullptr);
        }
    }
    ATPDXY_LOG_DEBUG(g_logger) << " Fiber::~Fiber id=" << m_id
        << " total=" << s_fiber_count;
}

// 重置协程执行函数
void Fiber::reset(std::function<void()> cb){
    ATPDXY_ASSERT(m_stack);
    ATPDXY_ASSERT(m_state ==TERM || m_state == EXCEPT || m_state == INIT);
    m_cb = cb;
    if(getcontext(&m_ctx)){
        ATPDXY_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

void Fiber::back(){
    SetThis(t_threadFiber.get());
    if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)){
        ATPDXY_ASSERT2(false, "swapcontext");
    }
}

void Fiber::call(){
    SetThis(this);
    m_state = EXEC;
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)){
        ATPDXY_ASSERT2(false, "swapcontext");
    }
}

// 将当前协程加入到处理器中开始执行
void Fiber::swapIn(){
    SetThis(this);
    ATPDXY_ASSERT(m_state != EXEC);
    m_state = EXEC;
    // 保存当前上下文并切换到目标上下文
    if(swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)){
        ATPDXY_ASSERT2(false, "swapcontext");
    }
}

// 将当前协程移出处理器中停止执行
void Fiber::swapOut(){
    SetThis(Scheduler::GetMainFiber());
    if(swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)){
        ATPDXY_ASSERT2(false, "swapcontext");
    }
}
// 返回当前执行点的协程
Fiber::ptr Fiber::GetThis(){
    if(t_fiber){
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    ATPDXY_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

// 协程切换到后台，将状态设置为Ready
void Fiber::YieldToReady(){
    Fiber::ptr cur = GetThis();
    ATPDXY_ASSERT(cur->m_state == EXEC);
    cur->m_state = READY;
    cur->swapOut();
}
// 协程切换到后台，将状态设置为Hold
void Fiber::YieldToHold(){
    Fiber::ptr cur = GetThis();
    ATPDXY_ASSERT(cur->m_state == EXEC);
    cur->m_state = HOLD;
    cur->swapOut();
}

// 获取总协程数
uint64_t Fiber::TotalFibers(){
    return s_fiber_count;
}

void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    ATPDXY_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        ATPDXY_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << atpdxy::BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        ATPDXY_LOG_ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << atpdxy::BacktraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();

    ATPDXY_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

void Fiber::CallerMainFunc() {
    Fiber::ptr cur = GetThis();
    ATPDXY_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        ATPDXY_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << atpdxy::BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        ATPDXY_LOG_ERROR(g_logger) << "Fiber Except"
            << " fiber_id=" << cur->getId()
            << std::endl
            << atpdxy::BacktraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();
    ATPDXY_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}


uint64_t Fiber::GetFiberId(){
    if(t_fiber){
        return t_fiber->getId();
    }
    return 0;
}

}