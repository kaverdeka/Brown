cmake_minimum_required(VERSION 3.20)
project(Textures)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(Textures main.cpp Solution.cpp Textures.cpp)

find_package(Threads REQUIRED)
target_link_libraries(Textures PRIVATE Threads::Threads)

