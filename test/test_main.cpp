// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <chrono>
#include <gtest/gtest.h>
#include "reqhv.hpp"

using namespace std::literals::chrono_literals;

TEST(ReqhvTest, SimpleGetRequest) {
    auto res = reqhv::get("https://httpbin.org/get").send();
    EXPECT_EQ(res.status_code(), 200);
    EXPECT_TRUE(res.is_success());
    EXPECT_FALSE(res.text().empty());
}

TEST(ReqhvTest, ClientBuilder) {
    auto client = reqhv::Client::builder()
        .timeout(10s)
        .user_agent("reqhv-test/1.0")
        .build();

    auto res = client.get("https://httpbin.org/get").send();
    EXPECT_EQ(res.status_code(), 200);
}

TEST(ReqhvTest, ResponseHeaders) {
    auto res = reqhv::get("https://httpbin.org/get").send();
    EXPECT_TRUE(res.headers().empty() == false);
}

TEST(ReqhvTest, StatusCode) {
    auto res = reqhv::get("https://httpbin.org/status/404").send();
    EXPECT_EQ(res.status_code(), 404);
    EXPECT_TRUE(res.is_client_error());
}

TEST(ReqhvTest, AsyncSuspendsAndWaits) {
    auto start = std::chrono::steady_clock::now();
    auto future = reqhv::get("https://httpbin.org/delay/1").send_async();
    auto res = future.get();
    auto end = std::chrono::steady_clock::now();

    EXPECT_EQ(res.status_code(), 200);
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    EXPECT_GE(elapsed_ms, 900) << "Should have waited for response";
}

TEST(ReqhvTest, ManyParallelAsyncRequests) {
    auto start = std::chrono::steady_clock::now();

    auto f1 = reqhv::get("https://httpbin.org/delay/1").send_async();
    auto f2 = reqhv::get("https://httpbin.org/delay/1").send_async();
    auto f3 = reqhv::get("https://httpbin.org/delay/1").send_async();
    auto f4 = reqhv::get("https://httpbin.org/delay/1").send_async();
    auto f5 = reqhv::get("https://httpbin.org/delay/1").send_async();

    auto res1 = f1.get();
    auto res2 = f2.get();
    auto res3 = f3.get();
    auto res4 = f4.get();
    auto res5 = f5.get();

    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_LT(elapsed_ms, 6000) << "Should complete in ~6s if parallel, was " << elapsed_ms << "ms";
    EXPECT_EQ(res1.status_code(), 200);
    EXPECT_EQ(res2.status_code(), 200);
    EXPECT_EQ(res3.status_code(), 200);
    EXPECT_EQ(res4.status_code(), 200);
    EXPECT_EQ(res5.status_code(), 200);
}

TEST(ReqhvTest, ManySequentialAsyncRequests) {
    auto start = std::chrono::steady_clock::now();

    auto res1 = reqhv::get("https://httpbin.org/delay/1").send();
    auto res2 = reqhv::get("https://httpbin.org/delay/1").send();
    auto res3 = reqhv::get("https://httpbin.org/delay/1").send();
    auto res4 = reqhv::get("https://httpbin.org/delay/1").send();
    auto res5 = reqhv::get("https://httpbin.org/delay/1").send();

    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_GE(15000, elapsed_ms) << "Should complete in ~15s if sequential, was " << elapsed_ms << "ms";
    EXPECT_EQ(res1.status_code(), 200);
    EXPECT_EQ(res2.status_code(), 200);
    EXPECT_EQ(res3.status_code(), 200);
    EXPECT_EQ(res4.status_code(), 200);
    EXPECT_EQ(res5.status_code(), 200);
}

TEST(ReqhvTest, AsyncPost) {
    nlohmann::json data = {{"name", "reqhv"}, {"version", "1.0"}};
    auto res = reqhv::post("https://httpbin.org/post")
        .json(data)
        .send_async()
        .get();
    EXPECT_EQ(res.status_code(), 200);
}
