cmake_minimum_required(VERSION 3.20)
project(ConcurrentMap2)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(ConcurrentMap2 concurrentMap.cpp)

find_package(Threads REQUIRED)
target_link_libraries(ConcurrentMap2 PRIVATE Threads::Threads)

