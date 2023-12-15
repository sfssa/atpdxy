#pragma once
#include <memory>
#include <vector>
#include <list>
#include "mutex.h"
#include "fiber.h"
#include "thread.h"

namespace atpdxy{

class Scheduler{
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;
    // 线程数目-将创建调度器的线程纳入线程调度器中-协程调度器的名称
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();
    const std::string& getName() const { return m_name;}
    // 获取当前协程调度器是哪一个
    static Scheduler* GetThis();
    // 获取协程调度器的主协程
    static Fiber* GetMainFiber();
    // 启动
    void start();
    // 停止
    void stop();
    template <class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1){
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }

        if(need_tickle){
            tickle();
        }
    }
    
    template <class InputIterator>
    void schedule(InputIterator begin, InputIterator end){
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            while(begin != end){
                need_tickle = scheduleNoLock(&*begin, -1)  || need_tickle;
                ++begin;
            }
        }
        
        if(need_tickle){
            tickle();
        }
    }
protected:
    virtual void tickle();
    void run();
    // 让子类有清理资源的机会
    virtual bool stopping();
    void setThis();
    virtual void idle();
private:
    template <class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread){
        // 新放入任务后返回true通知有任务来了
        bool need_tickle = m_tasks.empty();
        Task ft(fc, thread);
        if(ft.fiber || ft.cb){
            m_tasks.push_back(ft);
        }
        return need_tickle;
    }
private:
    struct Task{
    public:
        Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;
        Task(Fiber::ptr f, int thr)
            :fiber(f), thread(thr){
            
        }
        // 切换任务时使用，将一个已存在的协程转移给任务对象
        // 在不增加引用计数的情况下，将一个协程对象移动到另一个协程对象
        Task(Fiber::ptr* f, int thr)
            :thread(thr){
            // 将引用减一
            fiber.swap(*f);
        }

        Task(std::function<void()> f, int thr)
            :cb(f), thread(thr){

        }

        Task(std::function<void()>* f, int thr)
            :thread(thr){
            cb.swap(*f);
        }

        // 使用STL时能够初始化
        Task()
            :thread(-1){
        
        }

        void reset(){
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }

    };
private:
    MutexType m_mutex;
    std::vector<Thread::ptr> m_threads;
    // 放指针会不会更好
    std::list<Task> m_tasks;
    Fiber::ptr m_rootFiber;
    std::string m_name;
protected:
    // 协程下的id数组
    std::vector<int> m_threadIds;
    // 线程数量
    size_t m_threadCount = 0;
    // 工作线程数量
    std::atomic<size_t> m_activeThreadCount = {0};
    // 空闲线程数量
    std::atomic<size_t> m_idleThreadCount = {0};
    // 是否正在停止
    bool m_stopping = true;
    // 是否自动停止
    bool m_autoStop = false;
    // 主线程id
    int m_rootThread = 0;
};

}