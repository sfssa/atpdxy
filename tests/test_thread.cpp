#include "../atpdxy/atpdxy.h"
#include <vector>
#include <unistd.h>

atpdxy::Logger::ptr g_logger = ATPDXY_LOG_ROOT();

void test01(){
    ATPDXY_LOG_INFO(g_logger) << "thread_name=" << atpdxy::Thread::GetName()
        << " this.name=" << atpdxy::Thread::GetThis()->getName()
        << " id=" << atpdxy::GetThreadId()
        << " this.id=" << atpdxy::Thread::GetThis()->getId();
        sleep(20);
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
    
    return 0;
}