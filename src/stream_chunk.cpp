// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "stream_chunk.hpp"

namespace reqhv {

std::function<void(HttpMessage*, http_parser_state, const char*, size_t)> StreamChunk::http_cb() {
    return [this](HttpMessage* resp, http_parser_state state, const char* data, size_t size) {
        if (state == HP_HEADERS_COMPLETE) {
            auto it = resp->headers.find("Content-Length");
            if (it != resp->headers.end()) {
                try {
                    set_content_length(std::stoull(it->second));
                } catch (...) {}
            }
        } else if (state == HP_BODY && data && size) {
            add_chunk(data, size);
        } else if (state == HP_MESSAGE_COMPLETE) {
            set_finished();
        }
    };
}

std::optional<StreamChunk::Chunk> StreamChunk::next() {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this]() { return !chunks_.empty() || finished_; });
    if (!chunks_.empty()) {
        Chunk chunk = std::move(chunks_.front());
        chunks_.pop();
        return chunk;
    }
    return std::nullopt;
}

std::optional<StreamChunk::Chunk> StreamChunk::try_next() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!chunks_.empty()) {
        Chunk chunk = std::move(chunks_.front());
        chunks_.pop();
        return chunk;
    }
    return std::nullopt;
}

bool StreamChunk::is_finished() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return finished_;
}

size_t StreamChunk::received_bytes() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return total_received_;
}

std::optional<size_t> StreamChunk::content_length() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return content_length_;
}

void StreamChunk::add_chunk(const char* data, size_t size) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        chunks_.emplace(data, data + size);
        total_received_ += size;
    }
    cv_.notify_one();
}

void StreamChunk::set_content_length(size_t len) {
    std::lock_guard<std::mutex> lock(mtx_);
    content_length_ = len;
}

void StreamChunk::set_finished() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        finished_ = true;
    }
    cv_.notify_all();
}

} // namespace reqhv