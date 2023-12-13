#include "../atpdxy/atpdxy.h"
atpdxy::Logger::ptr g_logger = ATPDXY_LOG_ROOT();

void runInFiber(){
    ATPDXY_LOG_INFO(g_logger) << "run in fiber begin";
    atpdxy::Fiber::YieldToHold();
    ATPDXY_LOG_INFO(g_logger) << "run in fiber end";
    atpdxy::Fiber::YieldToHold();
}

void testFiber(){
    ATPDXY_LOG_INFO(g_logger) << "main begin -1";
    {
        atpdxy::Fiber::GetThis();
        ATPDXY_LOG_INFO(g_logger) << "main begin";
        atpdxy::Fiber::ptr fiber(new atpdxy::Fiber(runInFiber));
        fiber->swapIn();
        ATPDXY_LOG_INFO(g_logger) << "main afer swapIn";
        fiber->swapIn();
        ATPDXY_LOG_INFO(g_logger) << "main afer end";
        fiber->swapIn();
    }
    ATPDXY_LOG_INFO(g_logger) << "main afer end2";
}

int main(){
    atpdxy::Thread::SetName("main");
    std::vector<atpdxy::Thread::ptr> thrs;
    for(int i = 0; i < 3; ++i){
        thrs.push_back(atpdxy::Thread::ptr(
            new atpdxy::Thread(&testFiber, "name_" + std::to_string(i))));

    }
    for(auto i : thrs){
        i->join();
    }
    return 0;
}
