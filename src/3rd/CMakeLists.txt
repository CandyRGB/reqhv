# 3rd party dependencies
# Header-only libraries only

add_library(3rd INTERFACE
)
target_include_directories(3rd INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})