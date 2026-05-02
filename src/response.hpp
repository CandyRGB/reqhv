// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <optional>
#include <string>
#include <vector>
#include <cstddef>

#include <hv/HttpClient.h>
#include <hv/json.hpp>

#include "exception.hpp"

namespace reqhv {

template<bool EnableStream>
class RequestBuilder;

// HTTP 响应封装类
// 统一访问响应状态、Header、Body（文本/JSON/字节）
class Response {
public:
    // 状态码相关
    int status_code() const;             // 获取 HTTP 状态码
    bool is_success() const;             // 2xx 表示成功
    bool is_redirect() const;            // 3xx 重定向
    bool is_client_error() const;        // 4xx 客户端错误
    bool is_server_error() const;        // 5xx 服务器错误

    // Body 访问方式
    std::string text() const;            // 获取响应体文本（带缓存）
    std::vector<char> bytes();           // 获取响应体字节
    std::string_view text_view() const;  // 获取响应体视图（避免拷贝）
    nlohmann::json json() const;         // 解析 JSON 响应体

    // Header 访问
    const http_headers& headers() const;
    std::optional<std::string> header(std::string_view key) const;
    const std::string& url() const { return url_; }
    void set_url(const std::string& url) { url_ = url; }

    // Content-Length header
    std::optional<uint64_t> content_length() const;

    // 若状态码表示错误则抛出异常
    Response error_for_status();

    // 原始 HttpResponse 指针，用于高级用法
    HttpResponse* raw() const { return response_.get(); }
    explicit operator bool() const { return response_ != nullptr; }

private:
    Response() = delete;

    // 供 RequestBuilder 调用
    explicit Response(std::shared_ptr<HttpResponse> resp)
        : response_(std::move(resp)) {}

    std::shared_ptr<HttpResponse> response_;  // libhv 原始响应对象
    std::string url_;                         // 最终请求 URL（跟随重定向后）
    mutable std::string cached_text_;         // 文本缓存，避免重复转换

    template<bool> friend class RequestBuilder;
};

} // namespace reqhv