// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <string>
#include <vector>

#include <hv/httpdef.h>
#include <hv/HttpMessage.h>

namespace reqhv {

// 分块流式接收类 - 配合 RequestBuilder::receive_stream() 使用
class StreamChunk {
public:
    using Chunk = std::vector<char>;

    StreamChunk() = default;

    std::optional<Chunk> next();                    // 阻塞获取下一个 chunk
    std::optional<Chunk> try_next();                // 非阻塞尝试获取
    bool is_finished() const;                       // 是否已结束
    size_t received_bytes() const;                  // 已接收字节数
    std::optional<size_t> content_length() const;   // Content-Length

    // 获取 http_cb，供 RequestBuilder 使用
    std::function<void(HttpMessage*, http_parser_state, const char*, size_t)> http_cb();

private:
    void add_chunk(const char* data, size_t size);
    void set_content_length(size_t len);
    void set_finished();

    std::queue<Chunk> chunks_;
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    bool finished_ = false;
    size_t total_received_ = 0;
    std::optional<size_t> content_length_;
};

} // namespace reqhv