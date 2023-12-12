#include "../atpdxy/atpdxy.h"
#include <vector>
#include <unistd.h>

atpdxy::Logger::ptr g_logger = ATPDXY_LOG_ROOT();
// 使用volatile阻止编译器优化
volatile int count = 0;
atpdxy::RWMutex s_mutex;
atpdxy::Mutex mutex;
atpdxy::Spinlock spin_mutex;
atpdxy::CASLock cas_mutex;

void test01(){
    ATPDXY_LOG_INFO(g_logger) << "thread_name=" << atpdxy::Thread::GetName()
        << " this.name=" << atpdxy::Thread::GetThis()->getName()
        << " id=" << atpdxy::GetThreadId()
        << " this.id=" << atpdxy::Thread::GetThis()->getId();
        // sleep(20);
    for(int i = 0 ; i < 100000; ++i){
        // atpdxy::RWMutex::WriteLock lock(s_mutex);
        // atpdxy::Spinlock::Lock lock(spin_mutex);
        atpdxy::CASLock::Lock lock(cas_mutex);
        ++count;
    }
}

void test02(){
    while(true){
        ATPDXY_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }    
}

void test03(){
    while(true){
        ATPDXY_LOG_INFO(g_logger) << "===========================";
    }
}

int main(){
    ATPDXY_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/pzx/atpdxy/bin/conf/log2.yml");
    atpdxy::Config::LoadFromYaml(root);
    std::vector<atpdxy::Thread::ptr> thrs;
    for(size_t i = 0; i < 1; ++i){
        // atpdxy::Thread::ptr thr(new atpdxy::Thread(&test01, "name_" + std::to_string(i)));
        // thrs.push_back(thr);
        atpdxy::Thread::ptr thr1(new atpdxy::Thread(&test02, "name_" + std::to_string(i * 2)));
        // atpdxy::Thread::ptr thr2(new atpdxy::Thread(&test03, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr1);
        // thrs.push_back(thr2);
    }
    
    for(size_t i = 0; i < thrs.size(); ++i){
        thrs[i]->join();
    }
    ATPDXY_LOG_INFO(g_logger) << "thread test end";
    ATPDXY_LOG_INFO(g_logger) << "count=" <<count;
    return 0;
}