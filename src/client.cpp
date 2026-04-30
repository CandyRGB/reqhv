// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "request_builder.hpp"
#include "client.hpp"
#include "client_builder.hpp"

namespace reqhv {

Client Client::create() {
    return Client(Config{});
}

ClientBuilder Client::builder() {
    return ClientBuilder();
}

Client::Client(const Config& config)
    : config_(config) {
    http_client_.setTimeout(static_cast<int>(config_.timeout.count() / 1000));
    http_client_.setHeader("User-Agent", config_.user_agent.c_str());
    for (const auto& [key, value] : config_.default_headers) {
        http_client_.setHeader(key.c_str(), value.c_str());
    }
}

Client::Client(Client&& other) noexcept
    : config_(std::move(other.config_))
    , http_client_(std::move(other.http_client_)) {}

Client& Client::operator=(Client&& other) noexcept {
    if (this != &other) {
        config_ = std::move(other.config_);
        http_client_ = std::move(other.http_client_);
    }
    return *this;
}

RequestBuilder Client::get(const std::string& url) {
    return RequestBuilder(HTTP_GET, url, std::ref(*this));
}

RequestBuilder Client::post(const std::string& url) {
    return RequestBuilder(HTTP_POST, url, std::ref(*this));
}

RequestBuilder Client::put(const std::string& url) {
    return RequestBuilder(HTTP_PUT, url, std::ref(*this));
}

RequestBuilder Client::patch(const std::string& url) {
    return RequestBuilder(HTTP_PATCH, url, std::ref(*this));
}

RequestBuilder Client::delete_(const std::string& url) {
    return RequestBuilder(HTTP_DELETE, url, std::ref(*this));
}

RequestBuilder Client::head(const std::string& url) {
    return RequestBuilder(HTTP_HEAD, url, std::ref(*this));
}

RequestBuilder Client::request(http_method method, const std::string& url) {
    return RequestBuilder(method, url, std::ref(*this));
}

} // namespace reqhv