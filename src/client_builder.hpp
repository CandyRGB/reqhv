// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <initializer_list>

#include "config.hpp"

namespace reqhv {

class Client;

// Client 构建器，采用流式配置风格
// 用于创建配置好的 Client 实例
class ClientBuilder {
public:
    // 配置方法
    ClientBuilder& timeout(std::chrono::milliseconds ms);
    ClientBuilder& connect_timeout(std::chrono::milliseconds ms);
    ClientBuilder& user_agent(std::string_view ua);
    ClientBuilder& default_headers(const http_headers& headers);
    ClientBuilder& cookie_store(bool enable);
    ClientBuilder& cookie_path(const std::string& path);
    ClientBuilder& redirect(int max_redirects);
    ClientBuilder& gzip(bool enable);
    ClientBuilder& danger_accept_invalid_certs(bool accept);
    ClientBuilder& proxy(const std::string& host, int port);
    ClientBuilder& https_proxy(const std::string& host, int port);
    ClientBuilder& no_proxy(const std::vector<std::string>& hosts);
    ClientBuilder& no_proxy(std::initializer_list<std::string> hosts);

    // 构建 Client 实例
    class Client build();

private:
    // 供 Client 调用
    ClientBuilder() = default;

    Config config_;

    friend class Client;
};

} // namespace reqhv