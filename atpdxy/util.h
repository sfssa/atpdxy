#pragma once
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <iomanip>
#include <iostream>

namespace atpdxy{

// 获取线程id
pid_t GetThreadId();
// 获取协程id
uint32_t GetFiberId();

// 输出栈调用的过程
void BackTrace(std::vector<std::string>& bt, int size = 64, int skip = 1);
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

}