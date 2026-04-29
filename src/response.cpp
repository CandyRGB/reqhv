// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "response.hpp"

namespace reqhv {

int Response::status_code() const {
    return response_ ? response_->status_code : 0;
}

bool Response::is_success() const {
    auto code = status_code();
    return code >= 200 && code < 300;
}

bool Response::is_redirect() const {
    auto code = status_code();
    return code == 301 || code == 302 || code == 303 || code == 307 || code == 308;
}

bool Response::is_client_error() const {
    auto code = status_code();
    return code >= 400 && code < 500;
}

bool Response::is_server_error() const {
    auto code = status_code();
    return code >= 500 && code < 600;
}

std::string Response::text() const {
    if (!cached_text_.empty()) return cached_text_;
    if (response_) {
        cached_text_ = response_->body;
    }
    return cached_text_;
}

std::vector<char> Response::bytes() {
    auto& body = response_ ? response_->body : cached_text_;
    return std::vector<char>(body.begin(), body.end());
}

std::string_view Response::text_view() const {
    if (response_) {
        return response_->body;
    }
    return {};
}

nlohmann::json Response::json() const {
    auto body_str = text();
    try {
        return nlohmann::json::parse(body_str);
    } catch (const nlohmann::json::parse_error& e) {
        throw HttpException::decode(std::string("JSON parse error: ") + e.what());
    }
}

const http_headers& Response::headers() const {
    static http_headers empty;
    return response_ ? response_->headers : empty;
}

std::optional<std::string> Response::header(std::string_view key) const {
    if (!response_) return std::nullopt;
    auto it = response_->headers.find(std::string(key));
    if (it != response_->headers.end()) {
        return it->second;
    }
    return std::nullopt;
}

// 非 2xx 状态码时抛出异常，用于链式调用
Response Response::error_for_status() {
    auto code = status_code();
    if (code == 0) {
        throw HttpException::request(0, url_);
    }
    if (is_client_error() || is_server_error()) {
        throw HttpException::request(code, url_);
    }
    return *this;
}

} // namespace reqhv