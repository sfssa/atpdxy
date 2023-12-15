#include "scheduler.h"
#include "log.h"
#include "macro.h"

namespace atpdxy{
    
static atpdxy::Logger::ptr g_logger = ATPDXY_LOG_NAME("system");
// 当前线程的调度器
static thread_local Scheduler* t_scheduler = nullptr;
// 当前线程的主协程
static thread_local Fiber* t_scheduler_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name)
    :m_name(name){
    ATPDXY_ASSERT(threads > 0);
    if(use_caller){
        // 将当前线程的上下文设置为主协程
        atpdxy::Fiber::GetThis();
        --threads;
        // 在创建时一个线程只有一个调度器
        ATPDXY_ASSERT(GetThis() == nullptr);
        t_scheduler = this;
        // 调度器的主协程用来执行run函数
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        atpdxy::Thread::SetName(m_name);
        // 添加到调度器中执行
        t_scheduler_fiber = m_rootFiber.get();
        m_rootThread = atpdxy::GetThreadId();
        m_threadIds.push_back(m_rootThread);
    }else{
        m_rootThread = -1;
    }
    m_threadCount = threads;
}

Scheduler::~Scheduler(){
    ATPDXY_ASSERT(m_stopping);
    if(GetThis() == this){
        t_scheduler = nullptr;
    }
} 
Scheduler* Scheduler::GetThis(){
    return t_scheduler;
}

Fiber* Scheduler::GetMainFiber(){
    return t_scheduler_fiber;
}

void Scheduler::start(){
    // ATPDXY_LOG_INFO(g_logger) << "start";
    MutexType::Lock lock(m_mutex);
    if(!m_stopping){
        return;
    }
    m_stopping = false;
    ATPDXY_ASSERT(m_threads.empty());
    m_threads.resize(m_threadCount);
    for(size_t i = 0; i < m_threadCount; ++i){
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this)
            , m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }
    lock.unlock();
}

void Scheduler::stop(){
    m_autoStop = true;
    if(m_rootFiber 
       && m_threadCount == 0 
       && (m_rootFiber->getState() == Fiber::TERM 
           || m_rootFiber->getState() == Fiber::INIT)){
        
        ATPDXY_LOG_INFO(g_logger) << this << " stopped";
        m_stopping = true;

        if(stopping()){
            return;
        }
    }
    // bool exit_on_this_fiber = false;
    // 用了user_caller一定要在创建调度器的线程中去执行stop，否则在任意一个
    if(m_rootThread != -1){
        ATPDXY_ASSERT(GetThis() == this);
    }else{
        ATPDXY_ASSERT(GetThis() != this);
    }
    m_stopping = true;
    for(size_t i = 0; i < m_threadCount; ++i){
        // 线程唤醒
        tickle();
    }
    if(m_rootFiber){
        tickle();
    }

    if(m_rootFiber){
        if(!stopping()){
            m_rootFiber->call();
        }
    }
    std::vector<Thread::ptr> thrs;
    {
        MutexType::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }
    for(auto& i : thrs){
        i->join();
    }
}

void Scheduler::run(){
    ATPDXY_LOG_INFO(g_logger) << m_name << " run";
    setThis();
    // 线程id不等于主线程id
    if(atpdxy::GetThreadId() != m_rootThread){
        t_scheduler_fiber = Fiber::GetThis().get();
    }
    // 没有任务时占住CPU或者休眠一段时间（可以由子类来实现）
    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    // 回调函数协程
    Fiber::ptr cb_fiber;
    Task ft;
    while(true){
        ft.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            MutexType::Lock lock(m_mutex);
            auto it = m_tasks.begin();
            while(it != m_tasks.end()){
                // 指定在哪个线程执行
                if(it->thread != -1 && it->thread != atpdxy::GetThreadId()){
                    ++it;
                    tickle_me = true;
                    continue;
                }
                ATPDXY_ASSERT(it->fiber || it->cb);
                if(it->fiber && it->fiber->getState() == Fiber::EXEC){
                    ++it;
                    continue;
                }
                ft = *it;
                m_tasks.erase(it);
                ++m_activeThreadCount;
                is_active = true;
                break;
            }
            tickle_me |= it != m_tasks.end();
        }
        if(tickle_me){
            tickle();
        }

        if(ft.fiber && (ft.fiber->getState() != Fiber::TERM 
            && ft.fiber->getState() != Fiber::EXCEPT)){
            ft.fiber->swapIn();
            --m_activeThreadCount;
            if(ft.fiber->getState() == Fiber::READY){
                schedule(ft.fiber);
            }else if(ft.fiber->getState() != Fiber::TERM 
                && ft.fiber->getState() != Fiber::EXCEPT){
                ft.fiber->m_state = Fiber::HOLD;
            }
            ft.reset();
        }else if(ft.cb){
            if(cb_fiber){
                cb_fiber->reset(ft.cb);
            }else {
                cb_fiber.reset(new Fiber(ft.cb));
            }
            ft.reset();
            cb_fiber->swapIn();
            --m_activeThreadCount;
            if(cb_fiber->getState() == Fiber::READY){
                schedule(cb_fiber);
                cb_fiber.reset();
            }else if(cb_fiber->getState() == Fiber::EXCEPT
                || cb_fiber->getState() == Fiber::TERM){
                cb_fiber->reset(nullptr);
            }else{
                cb_fiber->m_state = Fiber::HOLD;
                cb_fiber.reset();
            }
        }else{
            if(is_active){
                --m_activeThreadCount;
                continue;
            }
            if(idle_fiber->getState() == Fiber::TERM){
                ATPDXY_LOG_INFO(g_logger) << "idle fiber term";
                break;
            }
            ++m_idleThreadCount;
            idle_fiber->swapIn();
            --m_idleThreadCount;
            if(idle_fiber->getState() != Fiber::TERM 
                && idle_fiber->getState() != Fiber::EXCEPT){
                idle_fiber->m_state = Fiber::HOLD;
            }
        }
    }
}

bool Scheduler::stopping(){
    MutexType::Lock lock(m_mutex);
    return m_autoStop && m_stopping
        && m_tasks.empty() && m_activeThreadCount == 0;
}

void Scheduler::tickle(){
    ATPDXY_LOG_INFO(g_logger) << "tickle";
}

void Scheduler::setThis(){
    t_scheduler = this;
}

void Scheduler::idle(){
    ATPDXY_LOG_INFO(g_logger) << "idle";
    while(!stopping()){
        atpdxy::Fiber::YieldToHold();
    }
}
}
