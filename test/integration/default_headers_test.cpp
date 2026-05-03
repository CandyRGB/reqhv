// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <gtest/gtest.h>
#include "reqhv.hpp"

using namespace std::literals::chrono_literals;

TEST(IntegrationDefaultHeaders, DefaultHeadersSent) {
    auto client = reqhv::Client::builder()
        .default_headers({{"X-Custom-Header", "custom-value"}})
        .build();

    auto res = client.get("https://httpbin.org/headers").send();
    EXPECT_EQ(res.status_code(), 200);

    auto json = res.json();
    EXPECT_EQ(json["headers"]["X-Custom-Header"], "custom-value");
}

TEST(IntegrationDefaultHeaders, MultipleDefaultHeaders) {
    auto client = reqhv::Client::builder()
        .default_headers({
            {"X-Header-A", "value-a"},
            {"X-Header-B", "value-b"}
        })
        .build();

    auto res = client.get("https://httpbin.org/headers").send();
    EXPECT_EQ(res.status_code(), 200);

    auto json = res.json();
    EXPECT_EQ(json["headers"]["X-Header-A"], "value-a");
    EXPECT_EQ(json["headers"]["X-Header-B"], "value-b");
}

TEST(IntegrationDefaultHeaders, DefaultHeadersWithRequestHeaders) {
    auto client = reqhv::Client::builder()
        .default_headers({{"X-Default", "default-value"}})
        .build();

    auto res = client.get("https://httpbin.org/headers")
        .header("X-Request-Header", "request-value")
        .send();
    EXPECT_EQ(res.status_code(), 200);

    auto json = res.json();
    EXPECT_EQ(json["headers"]["X-Default"], "default-value");
    EXPECT_EQ(json["headers"]["X-Request-Header"], "request-value");
}

TEST(IntegrationDefaultHeaders, DefaultHeadersOverrideNotApplied) {
    // 请求头会覆盖默认头
    auto client = reqhv::Client::builder()
        .default_headers({{"User-Agent", "default-ua"}})
        .build();

    auto res = client.get("https://httpbin.org/headers")
        .header("User-Agent", "request-ua")
        .send();
    EXPECT_EQ(res.status_code(), 200);

    auto json = res.json();
    EXPECT_EQ(json["headers"]["User-Agent"], "request-ua");
}