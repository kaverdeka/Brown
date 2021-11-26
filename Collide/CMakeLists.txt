cmake_minimum_required(VERSION 3.20)
project(Collide)

set(CMAKE_CXX_STANDARD 17)

add_executable(Collide geo2d.cpp geo2d.h game_object.h collide.cpp)