cmake_minimum_required(VERSION 3.0)
project(atpdxy)
include (cmake/utils.cmake)

set (CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

set(LIB_SRC
    atpdxy/log.cpp
    atpdxy/util.cpp
    atpdxy/config.cpp
    atpdxy/thread.cpp
    atpdxy/scheduler.cpp
    atpdxy/mutex.cpp
    atpdxy/fiber.cpp)

add_library(atpdxy SHARED ${LIB_SRC}) 
# 重定义__FILE__为相对路径
force_redefine_file_macro_for_sources(atpdxy)
find_library(YAMLCPP yaml-cpp)
find_library(PTHREAD pthread)

set (LIBS
    atpdxy
    pthread
    yaml-cpp)

message("***" ${LIBS})

add_executable(test_log tests/test_log.cpp)
add_dependencies(test_log atpdxy)
force_redefine_file_macro_for_sources(test_log)
target_link_libraries(test_log ${LIBS})

add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config atpdxy)
force_redefine_file_macro_for_sources(test_config)
target_link_libraries(test_config ${LIBS})

add_executable(test_thread tests/test_thread.cpp)
add_dependencies(test_thread atpdxy)
force_redefine_file_macro_for_sources(test_thread)
target_link_libraries(test_thread ${LIBS})

add_executable(test_util tests/test_util.cpp)
add_dependencies(test_util atpdxy)
force_redefine_file_macro_for_sources(test_util)
target_link_libraries(test_util ${LIBS})

add_executable(test_fiber tests/test_fiber.cpp)
add_dependencies(test_fiber atpdxy)
force_redefine_file_macro_for_sources(test_fiber)
target_link_libraries(test_fiber ${LIBS})

add_executable(test_scheduler tests/test_scheduler.cpp)
add_dependencies(test_scheduler atpdxy)
force_redefine_file_macro_for_sources(test_scheduler)
target_link_libraries(test_scheduler ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
