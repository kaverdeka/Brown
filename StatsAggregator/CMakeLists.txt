cmake_minimum_required(VERSION 3.20)
project(StatsAggregator)

set(CMAKE_CXX_STANDARD 17)

add_executable(StatsAggregator main.cpp stats_aggregator.cpp stats_aggregator_test.cpp)