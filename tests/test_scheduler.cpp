#include "atpdxy/atpdxy.h"
atpdxy::Logger::ptr g_logger = ATPDXY_LOG_ROOT();

void test_fiber() {
    static int s_count = 5;
    ATPDXY_LOG_INFO(g_logger) << "test in fiber s_count=" << s_count;

    sleep(1);
    if(--s_count >= 0) {
        atpdxy::Scheduler::GetThis()->schedule(&test_fiber, atpdxy::GetThreadId());
    }
}

int main() {
    ATPDXY_LOG_INFO(g_logger) << "main";
    atpdxy::Scheduler sc(3, false, "test");
    sc.start();
    sleep(2);
    ATPDXY_LOG_INFO(g_logger) << "schedule";
    sc.schedule(&test_fiber);
    sc.stop();
    ATPDXY_LOG_INFO(g_logger) << "over";
    return 0;
}