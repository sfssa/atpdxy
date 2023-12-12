#include "../atpdxy/atpdxy.h"
#include <vector>
#include <unistd.h>

atpdxy::Logger::ptr g_logger = ATPDXY_LOG_ROOT();
// 使用volatile阻止编译器优化
volatile int count = 0;
atpdxy::RWMutex s_mutex;

void test01(){
    ATPDXY_LOG_INFO(g_logger) << "thread_name=" << atpdxy::Thread::GetName()
        << " this.name=" << atpdxy::Thread::GetThis()->getName()
        << " id=" << atpdxy::GetThreadId()
        << " this.id=" << atpdxy::Thread::GetThis()->getId();
        // sleep(20);
    for(int i = 0 ; i < 100000; ++i){
        atpdxy::RWMutex::ReadLock lock(s_mutex);
        ++count;
    }
}

void test02(){

}

int main(){
    ATPDXY_LOG_INFO(g_logger) << "thread test begin";
    std::vector<atpdxy::Thread::ptr> thrs;
    for(size_t i = 0; i < 5; ++i){
        atpdxy::Thread::ptr thr(new atpdxy::Thread(&test01, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }
    
    for(size_t i = 0; i < thrs.size(); ++i){
        thrs[i]->join();
    }
    ATPDXY_LOG_INFO(g_logger) << "thread test end";
    ATPDXY_LOG_INFO(g_logger) << "count=" <<count;
    return 0;
}