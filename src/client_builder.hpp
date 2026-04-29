// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include <hv/HttpMessage.h>

namespace reqhv {

// Client 构建器，采用流式配置风格
// 用于创建配置好的 Client 实例
class ClientBuilder {
public:
    ClientBuilder();

    // 配置方法
    ClientBuilder& timeout(std::chrono::milliseconds ms);                    // 设置默认请求超时
    ClientBuilder& connect_timeout(std::chrono::milliseconds ms);          // 设置连接超时
    ClientBuilder& user_agent(std::string_view ua);                         // 设置 User-Agent
    ClientBuilder& default_headers(const http_headers& headers);            // 设置默认请求头
    ClientBuilder& cookie_store(bool enable = true);                         // 启用/禁用 Cookie 存储
    ClientBuilder& redirect(int max_redirects);                             // 设置最大重定向次数
    ClientBuilder& gzip(bool enable = true);                               // 启用/禁用 gzip 解压
    ClientBuilder& danger_accept_invalid_certs(bool accept = true);        // 接受无效证书（仅测试用）

    // 构建 Client 实例
    class Client build();

private:
    std::chrono::milliseconds timeout_{30000};          // 默认请求超时 30 秒
    std::chrono::milliseconds connect_timeout_{10000};  // 默认连接超时 10 秒
    std::string user_agent_{"reqhv/1.0"};               // 默认 User-Agent
    http_headers default_headers_;                      // 默认请求头
    bool cookie_store_{true};                           // 默认启用 Cookie
    int max_redirects_{10};                             // 默认最多 10 次重定向
    bool gzip_{true};                                   // 默认启用 gzip
    bool danger_accept_invalid_certs_{false};           // 默认不接受无效证书

    friend class Client;
};

} // namespace reqhv