// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <gtest/gtest.h>
#include "reqhv.hpp"

using namespace std::literals::chrono_literals;

TEST(IntegrationHttp, SimpleGetRequest) {
    auto res = reqhv::get("https://httpbin.org/get").send();
    EXPECT_EQ(res.status_code(), 200);
    EXPECT_TRUE(res.is_success());
    EXPECT_FALSE(res.text().empty());
}

TEST(IntegrationHttp, GetWithClientBuilder) {
    auto client = reqhv::Client::builder()
        .timeout(10s)
        .user_agent("reqhv-test/1.0")
        .build();

    auto res = client.get("https://httpbin.org/get").send();
    EXPECT_EQ(res.status_code(), 200);
}

TEST(IntegrationHttp, GetStatusCode) {
    auto res = reqhv::get("https://httpbin.org/status/404").send();
    EXPECT_EQ(res.status_code(), 404);
    EXPECT_TRUE(res.is_client_error());
}

TEST(IntegrationHttp, GetResponseHeaders) {
    auto res = reqhv::get("https://httpbin.org/get").send();
    EXPECT_TRUE(res.headers().empty() == false);
}

TEST(IntegrationHttp, PostJson) {
    nlohmann::json data = {{"name", "reqhv"}, {"version", "1.0"}};
    auto res = reqhv::post("https://httpbin.org/post")
        .json(data)
        .send();
    EXPECT_EQ(res.status_code(), 200);
    auto json = res.json();
    EXPECT_EQ(json["json"]["name"], "reqhv");
}

TEST(IntegrationHttp, PostFormUrlEncoded) {
    auto res = reqhv::post("https://httpbin.org/post")
        .form({{"key", "value"}})
        .send();
    EXPECT_EQ(res.status_code(), 200);
}

TEST(IntegrationHttp, PutRequest) {
    auto res = reqhv::put("https://httpbin.org/put").send();
    EXPECT_EQ(res.status_code(), 200);
}

TEST(IntegrationHttp, PatchRequest) {
    auto res = reqhv::patch("https://httpbin.org/patch")
        .json({{"field", "value"}})
        .send();
    EXPECT_EQ(res.status_code(), 200);
}

TEST(IntegrationHttp, DeleteRequest) {
    auto res = reqhv::delete_("https://httpbin.org/delete").send();
    EXPECT_EQ(res.status_code(), 200);
}

TEST(IntegrationHttp, HeadRequest) {
    auto res = reqhv::head("https://httpbin.org/get").send();
    EXPECT_EQ(res.status_code(), 200);
    EXPECT_TRUE(res.text().empty());
}