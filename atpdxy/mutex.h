#pragma once
#include <thread>
#include <functional>
#include <memory>
#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>
#include <atomic>
#include <list>
#include "noncopyable.h"

namespace atpdxy{

class Semaphore{
public:
    // 构造函数
    Semaphore(uint32_t count = 0);
    // 析构函数
    ~Semaphore();
    // 阻塞
    void wait();
    // 唤醒
    void notify();
private:
    // 禁止拷贝的发生
    Semaphore(const Semaphore&) = delete;
    Semaphore(Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;
private:    
    sem_t m_semaphore;
};

// RAII机制
template <class T>
struct ScopeLockImpl{
public:
    ScopeLockImpl(T& mutex)
        :m_mutex(mutex){
        m_mutex.lock();
        m_locked = true;
    }

    ~ScopeLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

template <class T>
struct ReadLockImpl{
public:
    ReadLockImpl(T& mutex)
        :m_mutex(mutex){
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

template <class T>
struct WriteLockImpl{
public:
    WriteLockImpl(T& mutex)
        :m_mutex(mutex){
        m_mutex.wrlock();
        m_locked = true;
    }

    ~WriteLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

class Mutex{
public:

private:
    
};

class RWMutex{
public:
    typedef ReadLockImpl<RWMutex> ReadLock;
    typedef WriteLockImpl<RWMutex> WriteLock;
    RWMutex(){
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex(){
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock(){
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock(){
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock(){
        pthread_rwlock_unlock(&m_lock);
    }
private:
    pthread_rwlock_t m_lock;
};

}