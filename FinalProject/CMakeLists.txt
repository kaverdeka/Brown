cmake_minimum_required(VERSION 3.20)
project(FinalProject)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(FinalProject main.cpp BusStops.cpp details.cpp Routes.cpp json.cpp graph.cpp GraphRouter.cpp)

find_package(Threads REQUIRED)
target_link_libraries(FinalProject PRIVATE Threads::Threads)

