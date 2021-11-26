cmake_minimum_required(VERSION 3.20)
project(Spendings)

set(CMAKE_CXX_STANDARD 17)

add_executable(Spendings test_ini.cpp xml.cpp json.cpp ini.cpp)