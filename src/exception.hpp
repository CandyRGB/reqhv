// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <exception>
#include <optional>
#include <string>

namespace reqhv {

// HTTP 客户端异常类型，包含错误分类和状态码信息
// 用于统一处理请求超时、连接失败、HTTP 错误等场景
class HttpException : public std::exception {
public:
    // 错误类型枚举
    enum class ErrorKind {
        None,       // 无错误
        Timeout,    // 请求超时
        Connect,    // 连接失败
        Request,    // HTTP 请求错误（4xx/5xx）
        Body,       // 响应体解析错误
        Decode,     // 解码错误（如 JSON 解析失败）
        Redirect,   // 重定向错误
    };

    HttpException() = default;
    HttpException(const std::string& msg, ErrorKind kind = ErrorKind::None, int status_code = 0)
        : message_(msg), kind_(kind), status_code_(status_code) {}

    // 返回错误描述字符串
    const char* what() const noexcept override { return message_.c_str(); }

    // 错误类型判断方法
    bool is_timeout() const { return kind_ == ErrorKind::Timeout; }
    bool is_connect() const { return kind_ == ErrorKind::Connect; }
    bool is_request() const { return kind_ == ErrorKind::Request; }
    bool is_body() const { return kind_ == ErrorKind::Body; }
    bool is_decode() const { return kind_ == ErrorKind::Decode; }
    bool is_redirect() const { return kind_ == ErrorKind::Redirect; }

    // 获取错误类型和状态码
    ErrorKind kind() const { return kind_; }
    std::optional<int> status_code() const {
        if (status_code_ > 0) return status_code_;
        return std::nullopt;
    }
    const std::string& url() const { return url_; }
    void set_url(const std::string& url) { url_ = url; }

    // 工厂方法：创建特定类型的异常
    static HttpException timeout(const std::string& url = {});
    static HttpException connect(const std::string& url = {});
    static HttpException request(int status_code, const std::string& url = {});
    static HttpException body(const std::string& msg);
    static HttpException decode(const std::string& msg);

private:
    std::string message_;               // 错误描述信息
    ErrorKind kind_ = ErrorKind::None;  // 错误类型
    int status_code_ = 0;               // HTTP 状态码（仅对 Request 类型有意义）
    std::string url_;                   // 请求 URL，用于错误定位
};

} // namespace reqhv