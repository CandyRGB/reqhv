// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <string>
#include <chrono>

#include <hv/HttpClient.h>
#include <hv/HttpMessage.h>

#include "config.hpp"
#include "client_builder.hpp"
#include "cookie_jar.hpp"

namespace reqhv {

template <bool EnableStream>
class RequestBuilder;

// HTTP 客户端类
class Client {
public:
    // 返回一个使用默认配置的 Client
    static Client& create();

    // 用于链式配置 Client
    static ClientBuilder builder();

    // HTTP 方法接口
    RequestBuilder<false> get(const std::string& url);
    RequestBuilder<false> post(const std::string& url);
    RequestBuilder<false> put(const std::string& url);
    RequestBuilder<false> patch(const std::string& url);
    RequestBuilder<false> delete_(const std::string& url);
    RequestBuilder<false> head(const std::string& url);
    RequestBuilder<false> request(http_method method, const std::string& url);

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
    bool cookie_store_enabled() const { return config_.cookie_store; }
    CookieJar& cookie_jar() { return cookie_jar_; }
    std::mutex& async_send_mutex() const { return async_send_mutex_; }

    // 全局锁：保护 libhv 的 g_ssl_ctx 线程安全问题
    static std::mutex& global_ssl_mutex();

    Config config_;
    hv::HttpClient http_client_;
    CookieJar cookie_jar_;

    // 发送异步请求锁：libhv 的单实例异步请求会填充到一个 EventLoop 里，但填充之前未保证线程安全
    mutable std::mutex async_send_mutex_;

    friend class ClientBuilder;
    template<bool> friend class RequestBuilder;
};

} // namespace reqhv
