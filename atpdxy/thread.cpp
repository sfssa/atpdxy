#include "thread.h"
#include "util.h"
#include "log.h"

namespace atpdxy{
// 指向当前线程，static表示静态的，在程序运行期间只有一个实例，生命周期和程序运行周期相同
// thread_local表示变量是局部存储的，每个线程有一份独立的拷贝，每个线程都有一个独立的实例
static thread_local Thread* t_thread = nullptr;
// 当前线程的名称
static thread_local std::string t_thread_name = "UNKNOW";
static atpdxy::Logger::ptr g_logger = ATPDXY_LOG_NAME("system");

// 返回当前线程
Thread* Thread::GetThis(){
    return t_thread;
}

// 获取当前线程名称（日志）
const std::string& Thread::GetName(){
    return t_thread_name;
}

// 设置线程名称
void Thread::SetName(const std::string& name){
    if(t_thread){
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

// 线程构造函数
Thread::Thread(std::function<void()> cb, const std::string& name)
    :m_cb(cb), m_name(name){
    if(name.empty()){
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(rt){
        ATPDXY_LOG_ERROR(g_logger) << "pthread_create thread fail, rt= " << rt 
            << " name= " << name;
        throw std::logic_error("pthread_create error");
    }
    // 默认信号量是0，在这里线程被阻塞知道绑定了run函数后被唤醒继续执行
    m_semaphore.wait();
}

// 析构函数
Thread::~Thread(){
    if(m_thread){
        pthread_detach(m_thread);
    }
}

void* Thread::run(void* arg){
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = atpdxy::GetThreadId();
    // 修改线程名称
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());
    std::function<void()> cb;
    // 避免一次拷贝，直接移交所有权
    cb.swap(thread->m_cb);
    thread->m_semaphore.notify();
    cb();
    return 0;
}

// 等待执行完毕
void Thread::join(){
    if(m_thread){
        int rt = pthread_join(m_thread, nullptr);
        if(rt){
            ATPDXY_LOG_ERROR(g_logger) << "pthread_join thread fail, rt= " << rt 
                << " name= " << m_name;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}


}