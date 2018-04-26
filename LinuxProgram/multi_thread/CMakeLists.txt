project(sctp)
cmake_minimum_required(VERSION 3.5)

find_package (Threads)

set(CMAKE_CXX_STANDARD 14)

add_executable(foo multithread.cpp)
target_link_libraries (foo ${CMAKE_THREAD_LIBS_INIT})
