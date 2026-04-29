// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <string>
#include <chrono>

#include <hv/HttpMessage.h>

#include "client_builder.hpp"
#include "request_builder.hpp"

namespace reqhv {

// HTTP 客户端类
// 提供同步 HTTP 请求接口，支持 GET/POST/PUT/DELETE 等方法
// 使用方式：Client::builder()->timeout(10s)->user_agent("...")->build()
class Client {
public:
    // 创建默认配置的 Client（使用静态全局实例）
    static Client create();
    // 创建 Builder 用于自定义配置
    static ClientBuilder builder();

    // HTTP 方法接口，返回 RequestBuilder
    RequestBuilder get(const std::string& url);
    RequestBuilder post(const std::string& url);
    RequestBuilder put(const std::string& url);
    RequestBuilder patch(const std::string& url);
    RequestBuilder delete_(const std::string& url);
    RequestBuilder head(const std::string& url);
    RequestBuilder request(http_method method, const std::string& url);

private:
    Client() = default;
    explicit Client(ClientBuilder& builder);

    std::chrono::milliseconds timeout_{30000};     // 默认超时 30 秒
    std::string user_agent_{"reqhv/1.0"};         // 默认 User-Agent
    http_headers default_headers_;               // 默认请求头
    int max_redirects_{10};                       // 默认最多 10 次重定向

    friend class ClientBuilder;
};

} // namespace reqhv