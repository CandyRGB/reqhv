cmake_minimum_required(VERSION 3.16)

add_executable(response main.cpp)
target_link_libraries(response PRIVATE reqhv)