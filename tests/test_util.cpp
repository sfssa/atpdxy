#include <assert.h>
#include "../atpdxy/atpdxy.h"

atpdxy::Logger::ptr g_logger = ATPDXY_LOG_ROOT();

void test_assert(){
    // ATPDXY_LOG_INFO(g_logger) << atpdxy::BacktraceToString(10, 2, "    ");
    // ATPDXY_ASSERT(false);
    ATPDXY_LOG_INFO(g_logger) << atpdxy::BacktraceToString(10);
    ATPDXY_ASSERT2(0 == 1, "abcdef sfa");
}

int main(){
    test_assert();
    return 0;
}