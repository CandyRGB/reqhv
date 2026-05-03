cmake_minimum_required(VERSION 3.16)
project(reqhv VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_EXAMPLE "Build examples" OFF)
option(BUILD_TESTING "Build tests" ON)

macro(find_or_fetch PACKAGE_NAME REPO_URL TAG)
    find_package(${PACKAGE_NAME} QUIET)
    if(NOT ${PACKAGE_NAME}_FOUND)
        FetchContent_Declare(
            ${PACKAGE_NAME}
            GIT_REPOSITORY ${REPO_URL}
            GIT_TAG        ${TAG}
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(${PACKAGE_NAME})
    endif()
endmacro()

# FetchContent for dependencies
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/.deps)
include(FetchContent)

# libhv
find_or_fetch(libhv https://gitee.com/libhv/libhv.git v1.3.4)

# 3rd party
add_subdirectory(src/3rd)

# reqhv library
add_library(reqhv STATIC
    src/reqhv.cpp
    src/client.cpp
    src/client_builder.cpp
    src/response.cpp
    src/exception.cpp
    src/cookie_jar.cpp
    src/stream_chunk.cpp
)

target_include_directories(reqhv PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(reqhv PUBLIC
    hv_static
    3rd
)

install(TARGETS reqhv
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Examples
if(BUILD_EXAMPLE)
    add_subdirectory(examples)
endif()

# Testing
if(BUILD_TESTING)
    enable_testing()

    # GoogleTest
    find_or_fetch(googletest https://gitee.com/mirrors/googletest.git v1.14.0)

    add_subdirectory(test)
endif()