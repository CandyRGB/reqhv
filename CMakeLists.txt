cmake_minimum_required(VERSION 3.16)
project(reqhv VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_EXAMPLE "Build examples" OFF)
option(BUILD_TESTING "Build tests" OFF)

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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/3rd>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/reqhv>
)

target_link_libraries(reqhv PUBLIC
    hv_static
)

include(CMakePackageConfigHelpers)

install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/reqhv
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/3rd/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/reqhv/3rd
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)

install(TARGETS reqhv
    EXPORT reqhvTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT reqhvTargets
    FILE reqhvTargets.cmake
    NAMESPACE reqhv::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/reqhv
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/reqhvConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(
    cmake/reqhvConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/reqhvConfig.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/reqhvConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/reqhvConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/reqhv
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