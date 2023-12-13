#pragma once

#include <string.h>
#include <assert.h>
#include "log.h"
#include "util.h"

#if defined __GNUC__ || defined __llvm__
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率成立
#   define ATPDXY_LIKELY(x)       __builtin_expect(!!(x), 1)
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率不成立
#   define ATPDXY_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
#   define ATPDXY_LIKELY(x)      (x)
#   define ATPDXY_UNLIKELY(x)      (x)
#endif

/// 断言宏封装
#define ATPDXY_ASSERT(x) \
    if(ATPDXY_UNLIKELY(!(x))) { \
        ATPDXY_LOG_ERROR(ATPDXY_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << atpdxy::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

/// 断言宏封装
#define ATPDXY_ASSERT2(x, w) \
    if(ATPDXY_UNLIKELY(!(x))) { \
        ATPDXY_LOG_ERROR(ATPDXY_LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << atpdxy::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }
