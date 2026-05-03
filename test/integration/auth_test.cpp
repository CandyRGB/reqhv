// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "reqhv.hpp"
#include "auth_test_server.hpp"

using namespace std::literals::chrono_literals;

class IntegrationAuthTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        server.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    static void TearDownTestSuite() {
        server.stop();
    }

private:
    static reqhv::test::AuthTestServer server;
};

reqhv::test::AuthTestServer IntegrationAuthTest::server(19998);

TEST_F(IntegrationAuthTest, BearerTokenWithAuth) {
    auto client = reqhv::Client::builder()
        .timeout(5s)
        .build();

    auto res = client.get("http://127.0.0.1:19998/bearer")
        .bearer_auth("my-secret-token")
        .send();
    EXPECT_EQ(res.status_code(), 200);

    auto body = res.text();
    EXPECT_TRUE(body.find("my-secret-token") != std::string::npos)
        << "Response body should contain token: " << body;
}

TEST_F(IntegrationAuthTest, BasicAuthWithCorrectCredentials) {
    auto client = reqhv::Client::builder()
        .timeout(5s)
        .build();

    auto res = client.get("http://127.0.0.1:19998/basic-auth")
        .basic_auth("user", "passwd")
        .send();
    EXPECT_EQ(res.status_code(), 200);

    auto body = res.text();
    EXPECT_TRUE(body.find("ok") != std::string::npos)
        << "Response body should contain 'ok': " << body;
}

TEST_F(IntegrationAuthTest, BasicAuthWithWrongCredentials) {
    auto client = reqhv::Client::builder()
        .timeout(5s)
        .build();

    auto res = client.get("http://127.0.0.1:19998/basic-auth")
        .basic_auth("wrong", "credentials")
        .send();

    // 服务器应该拒绝错误凭证
    auto body = res.text();
    EXPECT_TRUE(body.find("error") != std::string::npos
        || res.status_code() == 401)
        << "Should reject wrong credentials, got status=" << res.status_code()
        << " body=" << body;
}