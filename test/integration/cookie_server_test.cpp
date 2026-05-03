// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "reqhv.hpp"
#include "cookie_test_server.hpp"

using namespace std::literals::chrono_literals;

class IntegrationCookieServerTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        server.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    static void TearDownTestSuite() {
        server.stop();
    }

private:
    static reqhv::test::CookieTestServer server;
};

reqhv::test::CookieTestServer IntegrationCookieServerTest::server(19997);

TEST_F(IntegrationCookieServerTest, SetAndGetCookie) {
    auto client = reqhv::Client::builder()
        .timeout(5s)
        .cookie_store(true)
        .build();

    auto res1 = client.get("http://127.0.0.1:19997/set-cookie").send();
    EXPECT_EQ(res1.status_code(), 200);

    auto res2 = client.get("http://127.0.0.1:19997/get-cookies").send();
    EXPECT_EQ(res2.status_code(), 200);

    auto json = res2.json();
    ASSERT_TRUE(json.contains("cookies")) << "Response should contain 'cookies' key";
    EXPECT_EQ(json["cookies"]["test_cookie"], "test_value");
}