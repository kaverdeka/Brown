cmake_minimum_required(VERSION 3.20)
project(UniquePtr)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(UniquePtr unique_ptr.cpp)

find_package(Threads REQUIRED)
target_link_libraries(UniquePtr PRIVATE Threads::Threads)

