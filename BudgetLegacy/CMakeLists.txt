cmake_minimum_required(VERSION 3.20)
project(BudgetLegacy)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(BudgetLegacy main.cpp)

find_package(Threads REQUIRED)
target_link_libraries(BudgetLegacy PRIVATE Threads::Threads)

