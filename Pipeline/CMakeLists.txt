cmake_minimum_required(VERSION 3.20)
project(Pipeline)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(Pipeline pipeline.cpp)

find_package(Threads REQUIRED)
target_link_libraries(Pipeline PRIVATE Threads::Threads)

