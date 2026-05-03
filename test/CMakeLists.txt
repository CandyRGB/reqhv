cmake_minimum_required(VERSION 3.16)

include(GoogleTest)

# 单元测试 - CookieJar, StreamChunk（无需网络）
add_executable(reqhv_unit_test
    unit/cookie_jar_test.cpp
    unit/stream_chunk_test.cpp
)
target_link_libraries(reqhv_unit_test PRIVATE
    reqhv
    GTest::gtest_main
)
gtest_discover_tests(reqhv_unit_test)

# 集成测试 - HTTP 功能（需要网络）
add_executable(reqhv_integration_test
    integration/http_method_test.cpp
    integration/auth_test.cpp
    integration/auth_test_server.cpp
    integration/redirect_test.cpp
    integration/redirect_test_server.cpp
    integration/cookie_test.cpp
    integration/async_test.cpp
    integration/multithread_test.cpp
    integration/streaming_test.cpp
    integration/default_headers_test.cpp
    integration/cookie_server_test.cpp
    integration/cookie_test_server.cpp
)
target_link_libraries(reqhv_integration_test PRIVATE
    reqhv
    GTest::gtest_main
)
gtest_discover_tests(reqhv_integration_test)