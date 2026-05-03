// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "reqhv.hpp"
#include "redirect_test_server.hpp"

using namespace std::literals::chrono_literals;

class IntegrationRedirectTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        server.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    static void TearDownTestSuite() {
        server.stop();
    }

private:
    static reqhv::test::RedirectTestServer server;
};

reqhv::test::RedirectTestServer IntegrationRedirectTest::server(19991);

TEST_F(IntegrationRedirectTest, Redirect301Get) {
    auto res = reqhv::get("http://127.0.0.1:19991/redirect301").send();
    EXPECT_EQ(res.status_code(), 200);
}

TEST_F(IntegrationRedirectTest, Redirect302Get) {
    auto res = reqhv::get("http://127.0.0.1:19991/redirect302").send();
    EXPECT_EQ(res.status_code(), 200);
}

TEST_F(IntegrationRedirectTest, Redirect303MethodChange) {
    auto res = reqhv::post("http://127.0.0.1:19991/redirect303").send();
    EXPECT_EQ(res.status_code(), 200);
}

TEST_F(IntegrationRedirectTest, MultipleRedirects) {
    auto res = reqhv::get("http://127.0.0.1:19991/multi-redirect").send();
    EXPECT_EQ(res.status_code(), 200);
}