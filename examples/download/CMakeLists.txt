cmake_minimum_required(VERSION 3.16)

add_executable(download main.cpp)
target_link_libraries(download PRIVATE reqhv)