#include "util.h"
#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <execinfo.h>
#include <stdarg.h>
#include <map>
#include "log.h"

namespace atpdxy{
atpdxy::Logger::ptr g_logger = ATPDXY_LOG_NAME("system");

pid_t GetThreadId(){
    return syscall(SYS_gettid);
}

uint32_t GetFiberId(){
    return 0;
}

void BackTrace(std::vector<std::string>& bt, int size, int skip){

    void** array = (void**)malloc((sizeof(void*) * size));
    size_t s = ::backtrace(array, size);

    char** strings = backtrace_symbols(array, s);
    if(strings == NULL){
        ATPDXY_LOG_ERROR(g_logger) << "backtrace_symnols error";
        return;
    }
    for(size_t i = skip; i < s; ++i){
        bt.push_back(strings[i]);
    }
    free(strings);
    free(array);   
    // std::unique_ptr<void*[]> array(new void*[size]);
    // size_t s = ::backtrace(array.get(), size);

    // char** strings = backtrace_symbols(array.get(), s);
    // if (strings == nullptr) {
    //     ATPDXY_LOG_ERROR(g_logger) << "backtrace_symbols error";
    //     return;
    // }

    // try {
    //     for (size_t i = skip; i < s; ++i) {
    //         bt.push_back(strings[i]);
    //     }
    // } catch (...) {
    //     free(strings);
    //     throw; // rethrow the exception
    // }

    // free(strings); 
}

std::string BacktraceToString(int size, int skip, const std::string& prefix){
    std::vector<std::string> bt;
    BackTrace(bt, size, skip);
    std::stringstream ss;
    for(size_t i = 0; i < bt.size(); ++i){
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

}