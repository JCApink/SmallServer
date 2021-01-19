cmake_minimum_required(VERSION 2.8)
add_executable(testLogging testLogging.cc)
target_link_libraries(testLogging libserver_base)