// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <chrono>
#include <string>

#include <hv/HttpMessage.h>

namespace reqhv {

// Client 配置数据类
// 存储所有可配置的选项，供 ClientBuilder 和 Client 共用
struct Config {
    std::chrono::milliseconds timeout{30000};           // 请求超时 30 秒
    std::chrono::milliseconds connect_timeout{10000};   // 连接超时 10 秒
    std::string user_agent{"reqhv/1.0"};                // User-Agent
    http_headers default_headers;                       // 默认请求头
    bool cookie_store{true};                            // 启用 Cookie
    int max_redirects{10};                              // 最大重定向次数
    bool gzip{true};                                    // 启用 gzip
    bool danger_accept_invalid_certs{false};            // 接受无效证书
};

} // namespace reqhv
