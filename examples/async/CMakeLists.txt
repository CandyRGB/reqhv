cmake_minimum_required(VERSION 3.16)

add_executable(async main.cpp)
target_link_libraries(async PRIVATE reqhv)