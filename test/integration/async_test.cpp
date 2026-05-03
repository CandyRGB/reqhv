// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <gtest/gtest.h>
#include "reqhv.hpp"

using namespace std::literals::chrono_literals;

TEST(IntegrationAsync, AsyncGet) {
    auto future = reqhv::get("https://httpbin.org/get").send_async();
    auto res = future.get();
    EXPECT_EQ(res.status_code(), 200);
}

TEST(IntegrationAsync, AsyncPost) {
    nlohmann::json data = {{"name", "reqhv"}, {"version", "1.0"}};
    auto res = reqhv::post("https://httpbin.org/post")
        .json(data)
        .send_async()
        .get();
    EXPECT_EQ(res.status_code(), 200);
}

TEST(IntegrationAsync, MultipleParallelAsync) {
    auto f1 = reqhv::get("https://httpbin.org/get").send_async();
    auto f2 = reqhv::get("https://httpbin.org/headers").send_async();
    auto f3 = reqhv::get("https://httpbin.org/ip").send_async();

    auto res1 = f1.get();
    auto res2 = f2.get();
    auto res3 = f3.get();

    EXPECT_EQ(res1.status_code(), 200);
    EXPECT_EQ(res2.status_code(), 200);
    EXPECT_EQ(res3.status_code(), 200);
}