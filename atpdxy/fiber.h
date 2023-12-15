#pragma once 
#include <ucontext.h>
#include <memory>
#include <string.h>
#include <functional>
#include "thread.h"

// 协程相比于线程更加轻量级，切花更快
namespace atpdxy{
// 能够获取当前类的智能指针，允许一个对在生命周期内以智能指针的形式与其他对象共享
// 可以在内部通过shared_from_this获得指向自身的智能指针
class Fiber : public std::enable_shared_from_this<Fiber>{
friend class Scheduler;
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State{ 
        // 初始化
        INIT,
        // 阻塞
        HOLD,
        // 执行
        EXEC,
        // 执行完毕
        TERM,
        // 就绪态
        READY,
        // 出错状态
        EXCEPT
    };
public:
    void call();
    void back();
    // 获取协程执行状态
    State getState() const { return m_state;}
    // 设置当前协程
    static void SetThis(Fiber* f);
    // 构造，包含协程执行的函数和栈空间
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
    ~Fiber();
    // 重置协程执行函数
    void reset(std::function<void()> cb);
    // 将当前协程加入到处理器中开始执行
    void swapIn();
    // 将当前协程移出处理器中停止执行
    void swapOut(); 
    // 返回当前执行点的协程
    static Fiber::ptr GetThis();
    // 协程切换到后台，将状态设置为Ready
    static void YieldToReady();
    // 协程切换到后台，将状态设置为Hold
    static void YieldToHold();
    // 获取总协程数
    static uint64_t TotalFibers();
    // 执行函数
    static void MainFunc();
    static void CallerMainFunc();
    // 返回协程id
    static  uint64_t GetFiberId();
    uint64_t getId() const { return m_id;}
private: 
    // 协程ID
    uint64_t m_id = 0;
    // 协程栈大小
    uint32_t m_stacksize = 0;
    // 协程状态
    State m_state = INIT;
    // 协程上下文数据结构
    ucontext_t m_ctx;
    // 栈指针
    void* m_stack = nullptr;
    // 协程执行函数
    std::function<void()> m_cb;
    // 每个线程的第一个协程
    Fiber();
};

}