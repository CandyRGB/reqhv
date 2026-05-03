// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <gtest/gtest.h>
#include <atomic>
#include "reqhv.hpp"

using namespace std::literals::chrono_literals;

TEST(IntegrationStreaming, StreamChunkReceivesData) {
    auto stream_chunk = std::make_shared<reqhv::StreamChunk>();
    auto client = reqhv::Client::builder().timeout(30s).build();

    auto resp = client.get("https://httpbin.org/stream/10")
        .receive_stream(stream_chunk)
        .send();

    EXPECT_EQ(resp.status_code(), 200);

    size_t bytes_received = 0;
    while (auto chunk = stream_chunk->next()) {
        bytes_received += chunk->size();
    }

    EXPECT_GT(bytes_received, 0) << "Should have received streaming data";
}

TEST(IntegrationStreaming, MultipleClientsStreaming) {
    constexpr int num_clients = 4;

    std::vector<std::pair<std::string, std::string>> endpoints = {
        {"https://httpbin.org/stream/5", "httpbin stream"},
        {"https://httpbin.org/bytes/512", "httpbin bytes"},
        {"https://httpbin.org/drip?numbytes=256&duration=1&delay=0", "httpbin drip"},
        {"https://speed.cloudflare.com/__down?bytes=512", "cloudflare"}
    };

    std::atomic<int> success_count{0};
    std::vector<std::thread> clients;

    for (int i = 0; i < num_clients; ++i) {
        auto [url, name] = endpoints[i];
        clients.emplace_back([=, &success_count]() {
            try {
                auto client = reqhv::Client::builder().timeout(30s).build();
                auto stream = std::make_shared<reqhv::StreamChunk>();
                auto resp = client.get(url).receive_stream(stream).send();

                size_t bytes = 0;
                while (auto chunk = stream->next()) {
                    bytes += chunk->size();
                }

                if (resp.status_code() == 200 && bytes > 0) {
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