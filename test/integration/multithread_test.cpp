// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <gtest/gtest.h>
#include <atomic>
#include "reqhv.hpp"

using namespace std::literals::chrono_literals;

TEST(IntegrationMultiThread, MultiThreadSyncRequests) {
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

TEST(IntegrationMultiThread, MultiClientConcurrentAsync) {
    constexpr int num_clients = 4;
    std::vector<std::pair<std::string, std::string>> endpoints = {
        {"https://httpbin.org/get", "httpbin.org get"},
        {"https://httpbin.org/headers", "httpbin.org headers"},
        {"https://httpbin.org/uuid", "httpbin.org uuid"},
        {"https://httpbin.org/ip", "httpbin.org ip"}
    };

    std::vector<std::thread> clients;
    std::atomic<int> success_count{0};

    for (int i = 0; i < num_clients; ++i) {
        auto [url, name] = endpoints[i];
        clients.emplace_back([&, url, name]() {
            try {
                auto client = reqhv::Client::builder().timeout(30s).build();
                auto future = client.get(url).send_async();
                auto resp = future.get();
                if (resp.status_code() == 200) {
                    success_count.fetch_add(1, std::memory_order_relaxed);
                }
            } catch (const std::exception&) {
            }
        });
    }

    for (auto& c : clients) {
        c.join();
    }

    EXPECT_EQ(success_count.load(), num_clients);
}