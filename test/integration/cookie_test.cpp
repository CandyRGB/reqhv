// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <gtest/gtest.h>
#include "reqhv.hpp"

using namespace std::literals::chrono_literals;

TEST(IntegrationCookie, CookieEnabled) {
    auto client = reqhv::Client::builder()
        .timeout(10s)
        .cookie_store(true)
        .build();

    auto res = client.get("https://httpbin.org/get").send();
    EXPECT_EQ(res.status_code(), 200);
}

TEST(IntegrationCookie, CookieDisabled) {
    auto client = reqhv::Client::builder()
        .timeout(10s)
        .cookie_store(false)
        .build();

    auto res = client.get("https://httpbin.org/cookies/set/disable_cookie/disable_value").send();
    EXPECT_EQ(res.status_code(), 200);

    auto res2 = client.get("https://httpbin.org/cookies").send();
    EXPECT_EQ(res2.status_code(), 200);
    auto json = res2.json();
    EXPECT_TRUE(json["cookies"].empty()) << "Cookies should not be stored when cookie_store is disabled";
}

TEST(IntegrationCookie, IndependentCookieJars) {
    auto client1 = reqhv::Client::builder()
        .timeout(10s)
        .cookie_store(true)
        .build();

    auto client2 = reqhv::Client::builder()
        .timeout(10s)
        .cookie_store(true)
        .build();

    auto res1 = client1.get("https://httpbin.org/cookies/set/client1_cookie/client1_value").send();
    EXPECT_EQ(res1.status_code(), 200);

    auto res2 = client2.get("https://httpbin.org/cookies").send();
    EXPECT_EQ(res2.status_code(), 200);
    auto json2 = res2.json();
    EXPECT_TRUE(json2["cookies"].empty()) << "Different clients should have independent cookie jars";
}