// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <memory>

#include <future>

#include <hv/HttpMessage.h>
#include <hv/httpdef.h>
#include <json.hpp>

#include "response.hpp"

namespace reqhv {

// RequestBuilder
class RequestBuilder {
public:
    RequestBuilder(http_method method, const std::string& url);

    // 链式配置方法
    RequestBuilder& header(std::string_view key, std::string_view value);
    RequestBuilder& headers(const http_headers& headers);
    RequestBuilder& body(std::string data);
    RequestBuilder& body(const char* data, size_t len);
    RequestBuilder& json(const nlohmann::json& data);
    RequestBuilder& form(const std::map<std::string, std::string>& data);
    RequestBuilder& query(const std::string& params);
    RequestBuilder& bearer_auth(const std::string& token);
    RequestBuilder& basic_auth(const std::string& user, const std::string& pass);
    RequestBuilder& timeout(std::chrono::milliseconds ms);

    // 同步发送，自动处理重定向
    Response send();

    // 低层级同步发送，不处理重定向
    Response send_sync();

    // 异步发送 - 返回 std::future<Response>
    std::future<Response> send_async();

    HttpRequest* raw_request() { return request_.get(); }

private:
    std::shared_ptr<HttpRequest> request_;
    std::chrono::milliseconds timeout_{30000};
    std::string url_;
    mutable int redirect_count_ = 0;
    int max_redirects_ = 10;

    Response do_send();

    friend class Client;
};

} // namespace reqhv
