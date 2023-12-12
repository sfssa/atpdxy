#pragma once
#include <thread>
#include <functional>
#include <memory>
#include <pthread.h>
#include "mutex.h"

namespace atpdxy{

class Thread{
public:
    // 智能指针
    typedef std::shared_ptr<Thread> ptr;
    // 线程构造函数
    Thread(std::function<void()> cb, const std::string& name);
    // 析构函数
    ~Thread();
    // 获取线程id
    pid_t getId() { return m_id;}
    // 获取线程名称
    const std::string& getName() const { return m_name;}
    // 等待执行完毕
    void join();
    // 返回当前线程
    static Thread* GetThis();
    // 获取当前线程名称（日志）
    static const std::string& GetName();
    // 设置线程的名称
    static void SetName(const std::string& name);
private:
    Thread(const Thread&) = delete;
    Thread(Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;
    Thread& operator=(Thread&&) = delete;
    static void* run(void* arg);
private:
    // 进程id
    pid_t m_id = -1;
    // 线程id
    pthread_t m_thread = 0;
    // 线程回调函数
    std::function<void()> m_cb;
    // 线程的名称
    std::string m_name;
    // 信号量
    Semaphore m_semaphore;
};

}