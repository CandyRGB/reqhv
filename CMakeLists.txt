cmake_minimum_required(VERSION 3.16)
project(reqhv VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_TESTING "Build tests" ON)

# FetchContent for dependencies
include(FetchContent)

# libhv
FetchContent_Declare(
    libhv
    GIT_REPOSITORY https://gitee.com/libhv/libhv.git
    GIT_TAG v1.3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(libhv)

# 3rd party
add_subdirectory(src/3rd)

# reqhv library
add_library(reqhv STATIC
    src/reqhv.cpp
    src/client.cpp
    src/client_builder.cpp
    src/request_builder.cpp
    src/response.cpp
    src/exception.cpp
)

target_include_directories(reqhv PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(reqhv PRIVATE
    hv_static
    3rd
)

# Testing
if(BUILD_TESTING)
    enable_testing()

    # GoogleTest
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(test)
endif()