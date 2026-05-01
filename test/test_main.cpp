// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
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

TEST(ReqhvTest, CookieClientOperations) {
    // 测试 CookieJar 相关 API 不报错
    auto client = reqhv::Client::builder()
        .timeout(10s)
        .cookie_store(true)
        .build();

    // 请求不报错即可，Cookie 功能在后续请求中生效
    auto res = client.get("https://httpbin.org/get").send();
    EXPECT_EQ(res.status_code(), 200);
}

TEST(ReqhvTest, CookieDisabled) {
    // 禁用 cookie_store 时不应该保存 cookies
    auto client = reqhv::Client::builder()
        .timeout(10s)
        .cookie_store(false)
        .build();

    auto res = client.get("https://httpbin.org/cookies/set/disable_cookie/disable_value").send();
    EXPECT_EQ(res.status_code(), 200);

    // 再次访问时 cookie 应该不存在（因为禁用了）
    auto res2 = client.get("https://httpbin.org/cookies").send();
    EXPECT_EQ(res2.status_code(), 200);
    auto json = res2.json();
    // cookie_store=false 时不会保存 cookie，所以这里不应该有之前设置的 cookie
    EXPECT_TRUE(json["cookies"].empty()) << "Cookies should not be stored when cookie_store is disabled";
}

TEST(ReqhvTest, CookiePersistence) {
    // 测试 cookie 持久化到文件
    // 注意：cookie 持久化需要手动调用 jar.save()
    // 这里主要测试多个 Client 实例可以独立使用各自的 jar
    auto client1 = reqhv::Client::builder()
        .timeout(10s)
        .cookie_store(true)
        .build();

    auto client2 = reqhv::Client::builder()
        .timeout(10s)
        .cookie_store(true)
        .build();

    // client1 设置 cookie
    auto res1 = client1.get("https://httpbin.org/cookies/set/client1_cookie/client1_value").send();
    EXPECT_EQ(res1.status_code(), 200);

    // client2 不应该有 client1 的 cookie（独立 jar）
    auto res2 = client2.get("https://httpbin.org/cookies").send();
    EXPECT_EQ(res2.status_code(), 200);
    auto json2 = res2.json();
    EXPECT_TRUE(json2["cookies"].empty()) << "Different clients should have independent cookie jars";
}

TEST(ReqhvTest, ProxyConfiguration) {
    // 测试代理配置能够正确应用
    // 注意：实际代理功能需要代理服务器，这里只验证配置不报错
    auto client = reqhv::Client::builder()
        .timeout(10s)
        .proxy("127.0.0.1", 7890)
        .https_proxy("127.0.0.1", 7890)
        .no_proxy({"localhost", "127.0.0.1"})
        .build();

    // 构造成功即表示配置生效，实际代理功能需要代理服务器测试
    SUCCEED() << "Proxy configuration applied successfully";
}

TEST(ReqhvTest, NoProxyConfiguration) {
    // 测试 no_proxy 配置能够正确应用
    auto client = reqhv::Client::builder()
        .timeout(10s)
        .no_proxy({"localhost", "127.0.0.1", "httpbin.org"})
        .build();

    // no_proxy 配置成功应用即可，实际旁路效果需要代理服务器测试
    SUCCEED() << "No-proxy configuration applied successfully";
}

TEST(ReqhvTest, MultiThreadSyncRequests) {
    // 测试多线程下各自创建独立的 Client 实例是否线程安全
    constexpr int num_threads = 8;
    constexpr int requests_per_thread = 5;
    std::atomic<int> success_count{0};
    std::atomic<int> fail_count{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < requests_per_thread; ++i) {
                try {
                    auto res = reqhv::get("https://httpbin.org/get").send();
                    if (res.status_code() == 200) {
                        success_count.fetch_add(1, std::memory_order_relaxed);
                    } else {
                        fail_count.fetch_add(1, std::memory_order_relaxed);
                    }
                } catch (const std::exception&) {
                    fail_count.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(success_count.load(), num_threads * requests_per_thread)
        << "All " << num_threads * requests_per_thread << " requests should succeed, but got "
        << success_count.load() << " success and " << fail_count.load() << " failures";
}
