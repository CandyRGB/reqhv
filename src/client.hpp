// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <string>
#include <chrono>

#include <hv/HttpClient.h>
#include <hv/HttpMessage.h>

#include "config.hpp"
#include "client_builder.hpp"
#include "request_builder.hpp"
#include "cookie_jar.hpp"

namespace reqhv {

// HTTP 客户端类
class Client {
public:
    // 返回一个使用默认配置的 Client
    static Client create();

    // 用于链式配置 Client
    static ClientBuilder builder();

    // HTTP 方法接口
    RequestBuilder get(const std::string& url);
    RequestBuilder post(const std::string& url);
    RequestBuilder put(const std::string& url);
    RequestBuilder patch(const std::string& url);
    RequestBuilder delete_(const std::string& url);
    RequestBuilder head(const std::string& url);
    RequestBuilder request(http_method method, const std::string& url);

private:
    Client() = delete;

    // 供 ClientBuilder 调用
    explicit Client(const Config& config);

    // Non-copyable
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    // Movable
    Client(Client&&) noexcept;
    Client& operator=(Client&&) noexcept;

    // 供 RequestBuilder 调用
    hv::HttpClient& http_client() { return http_client_; }
    int max_redirects() const { return config_.max_redirects; }
    CookieJar& cookie_jar() { return cookie_jar_; }
    bool cookie_store_enabled() const { return config_.cookie_store; }

    Config config_;
    hv::HttpClient http_client_;
    CookieJar cookie_jar_;

    friend class ClientBuilder;
    friend class RequestBuilder;
};

} // namespace reqhv
