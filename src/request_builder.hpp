// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <functional>
#include <string>
#include <memory>
#include <future>

#include <hv/HttpMessage.h>
#include <hv/httpdef.h>
#include <json.hpp>

#include "response.hpp"

namespace reqhv {

class Client;

class RequestBuilder {
public:
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

    // 同步发送，自动处理重定向
    Response send();

    // 异步发送，不支持重定向
    std::future<Response> send_async();

    HttpRequest* raw_request() { return request_.get(); }

private:
    RequestBuilder() = delete;

    // 供 Client 调用
    RequestBuilder(http_method method, const std::string& url, std::reference_wrapper<Client> client);
    
    std::shared_ptr<HttpRequest> request_;
    std::reference_wrapper<Client> client_;
    std::string url_;
    mutable int redirect_count_ = 0;
    // 发送异步请求锁：libhv 的单实例异步请求会填充到一个 EventLoop 里，但填充之前未保证线程安全
    mutable std::mutex async_send_mutex_;   

    Response do_send();

    friend class Client;
};

} // namespace reqhv
