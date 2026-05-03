// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <chrono>
#include "stream_chunk.hpp"

using namespace reqhv;

TEST(StreamChunk, InitialState) {
    StreamChunk sc;
    EXPECT_TRUE(sc.is_finished() == false);
    EXPECT_EQ(sc.received_bytes(), 0);
    EXPECT_FALSE(sc.content_length().has_value());
}

TEST(StreamChunk, TryNextOnEmptyReturnsNullopt) {
    StreamChunk sc;
    EXPECT_EQ(sc.try_next(), std::nullopt);
}

TEST(StreamChunk, HttpCbAddsChunk) {
    StreamChunk sc;
    auto cb = sc.http_cb();

    // 模拟 HTTP BODY 数据
    cb(nullptr, HP_BODY, "hello", 5);

    EXPECT_EQ(sc.received_bytes(), 5);
    auto chunk = sc.try_next();
    ASSERT_TRUE(chunk.has_value());
    EXPECT_EQ(chunk->size(), 5);
    EXPECT_EQ(std::string(chunk->begin(), chunk->end()), "hello");
}

TEST(StreamChunk, FifoOrder) {
    StreamChunk sc;
    auto cb = sc.http_cb();

    cb(nullptr, HP_BODY, "first", 5);
    cb(nullptr, HP_BODY, "second", 6);

    auto c1 = sc.try_next();
    auto c2 = sc.try_next();
    ASSERT_TRUE(c1.has_value());
    ASSERT_TRUE(c2.has_value());
    EXPECT_EQ(std::string(c1->begin(), c1->end()), "first");
    EXPECT_EQ(std::string(c2->begin(), c2->end()), "second");
}

TEST(StreamChunk, ContentLengthFromHeaders) {
    StreamChunk sc;
    auto cb = sc.http_cb();

    HttpResponse resp;
    resp.headers["Content-Length"] = "100";
    cb(&resp, HP_HEADERS_COMPLETE, nullptr, 0);

    ASSERT_TRUE(sc.content_length().has_value());
    EXPECT_EQ(sc.content_length().value(), 100);
}

TEST(StreamChunk, MessageCompleteSetsFinished) {
    StreamChunk sc;
    auto cb = sc.http_cb();

    cb(nullptr, HP_BODY, "data", 4);
    cb(nullptr, HP_MESSAGE_COMPLETE, nullptr, 0);

    EXPECT_TRUE(sc.is_finished());
    EXPECT_EQ(sc.received_bytes(), 4);

    // try_next 非阻塞，队列中还有数据，先取出
    auto chunk = sc.try_next();
    ASSERT_TRUE(chunk.has_value());
    EXPECT_EQ(chunk->size(), 4);

    // 队列空后再 try_next 返回 nullopt
    EXPECT_EQ(sc.try_next(), std::nullopt);

    // next() 在 finished 且队列空时返回 nullopt
    EXPECT_EQ(sc.next(), std::nullopt);
}

TEST(StreamChunk, ReceivedBytesAccumulates) {
    StreamChunk sc;
    auto cb = sc.http_cb();

    cb(nullptr, HP_BODY, "1234567890", 10);
    EXPECT_EQ(sc.received_bytes(), 10);

    cb(nullptr, HP_BODY, "abcdefghij", 10);
    EXPECT_EQ(sc.received_bytes(), 20);
}

TEST(StreamChunk, MultipleChunksOrder) {
    StreamChunk sc;
    auto cb = sc.http_cb();

    for (int i = 0; i < 10; ++i) {
        cb(nullptr, HP_BODY, "x", 1);
    }

    EXPECT_EQ(sc.received_bytes(), 10);

    int count = 0;
    while (auto chunk = sc.try_next()) {
        ++count;
    }
    EXPECT_EQ(count, 10);
}

TEST(StreamChunk, ThreadSafetySingleProducer) {
    StreamChunk sc;
    auto cb = sc.http_cb();
    std::atomic<int> chunks_added{0};

    std::thread t([&]() {
        for (int i = 0; i < 100; ++i) {
            cb(nullptr, HP_BODY, "x", 1);
            ++chunks_added;
        }
        cb(nullptr, HP_MESSAGE_COMPLETE, nullptr, 0);
    });

    t.join();
    EXPECT_EQ(chunks_added.load(), 100);
    EXPECT_TRUE(sc.is_finished());
    EXPECT_EQ(sc.received_bytes(), 100);
}

TEST(StreamChunk, NextBlocksUntilData) {
    StreamChunk sc;
    auto cb = sc.http_cb();

    std::thread t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        cb(nullptr, HP_BODY, "hello", 5);
    });

    auto chunk = sc.next();
    t.join();

    ASSERT_TRUE(chunk.has_value());
    EXPECT_EQ(std::string(chunk->begin(), chunk->end()), "hello");
}

TEST(StreamChunk, NextBlocksUntilFinished) {
    StreamChunk sc;
    auto cb = sc.http_cb();

    std::thread t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        cb(nullptr, HP_MESSAGE_COMPLETE, nullptr, 0);
    });

    auto result = sc.next();
    t.join();

    EXPECT_EQ(result, std::nullopt);
    EXPECT_TRUE(sc.is_finished());
}

TEST(StreamChunk, MultipleChunksNextVsTryNext) {
    StreamChunk sc;
    auto cb = sc.http_cb();

    cb(nullptr, HP_BODY, "chunk1", 6);
    cb(nullptr, HP_BODY, "chunk2", 6);

    auto c1 = sc.try_next();
    auto c2 = sc.try_next();
    auto c3 = sc.try_next();

    ASSERT_TRUE(c1.has_value());
    ASSERT_TRUE(c2.has_value());
    EXPECT_FALSE(c3.has_value());
}

TEST(StreamChunk, HttpCbIgnoresNullData) {
    StreamChunk sc;
    auto cb = sc.http_cb();

    // HP_BODY with null data should not add chunk
    cb(nullptr, HP_BODY, nullptr, 0);
    EXPECT_EQ(sc.received_bytes(), 0);
}