#include "util.h"
#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>

namespace atpdxy{

pid_t GetThreadId(){
    return syscall(SYS_gettid);
}

}