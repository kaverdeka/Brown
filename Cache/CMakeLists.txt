cmake_minimum_required(VERSION 3.20)
project(Cache)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(Cache main.cpp Solution.cpp)

find_package(Threads REQUIRED)
target_link_libraries(Cache PRIVATE Threads::Threads)

