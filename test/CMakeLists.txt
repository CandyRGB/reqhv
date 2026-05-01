cmake_minimum_required(VERSION 3.16)

include(GoogleTest)

add_executable(reqhv_test
    test_main.cpp
)

target_link_libraries(reqhv_test PRIVATE
    reqhv
    GTest::gtest_main
)
target_include_directories(reqhv_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/3rd
    ${libhv_SOURCE_DIR}/include
)

gtest_discover_tests(reqhv_test)