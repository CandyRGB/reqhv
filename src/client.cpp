// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "client.hpp"

#include <hv/hssl.h>

#include "client_builder.hpp"
#include "request_builder.hpp"

namespace reqhv {

Client& Client::create() {
    static Client client(Config{});
    return client;
}

ClientBuilder Client::builder() {
    return ClientBuilder();
}

Client::Client(const Config& config)
    : config_(config) {
    http_client_.setTimeout(static_cast<int>(config_.timeout.count() / 1000));
    http_client_.setHeader("User-Agent", config_.user_agent.c_str());
    for (const auto& [key, value] : config_.default_headers) {
        http_client_.setHeader(key.c_str(), value.c_str());
    }
    if (config_.cookie_store && !config_.cookie_file_path.empty()) {
        cookie_jar_.load(config_.cookie_file_path);
    }
    // 应用代理配置
    if (!config_.proxy_host.empty()) {
        http_client_.setHttpProxy(config_.proxy_host.c_str(), config_.proxy_port);
    }
    if (!config_.https_proxy_host.empty()) {
        http_client_.setHttpsProxy(config_.https_proxy_host.c_str(), config_.https_proxy_port);
    }
    for (const auto& h : config_.no_proxy) {
        http_client_.addNoProxy(h.c_str());
    }
    // 应用 TLS 配置
    bool has_tls_config = !config_.root_cert_pem.empty()    ||
                          !config_.client_cert_pem.empty()  ||
                          !config_.client_key_pem.empty()   ||
                          config_.danger_accept_invalid_certs;
    if (has_tls_config) {
        hssl_ctx_opt_t opt = {};
        opt.ca_file = config_.root_cert_pem.empty() ? nullptr : config_.root_cert_pem.c_str();
        opt.crt_file = config_.client_cert_pem.empty() ? nullptr : config_.client_cert_pem.c_str();
        opt.key_file = config_.client_key_pem.empty() ? nullptr : config_.client_key_pem.c_str();
        opt.verify_peer = config_.danger_accept_invalid_certs ? 0 : 1;
        http_client_.newSslCtx(&opt);
    }
}

Client::Client(Client&& other) noexcept
    : config_(std::move(other.config_))
    , http_client_(std::move(other.http_client_)) {}

Client& Client::operator=(Client&& other) noexcept {
    if (this != &other) {
        config_ = std::move(other.config_);
        http_client_ = std::move(other.http_client_);
    }
    return *this;
}

RequestBuilder<false> Client::get(const std::string& url) {
    return RequestBuilder<false>(HTTP_GET, url, std::ref(*this));
}

RequestBuilder<false> Client::post(const std::string& url) {
    return RequestBuilder<false>(HTTP_POST, url, std::ref(*this));
}

RequestBuilder<false> Client::put(const std::string& url) {
    return RequestBuilder<false>(HTTP_PUT, url, std::ref(*this));
}

RequestBuilder<false> Client::patch(const std::string& url) {
    return RequestBuilder<false>(HTTP_PATCH, url, std::ref(*this));
}

RequestBuilder<false> Client::delete_(const std::string& url) {
    return RequestBuilder<false>(HTTP_DELETE, url, std::ref(*this));
}

RequestBuilder<false> Client::head(const std::string& url) {
    return RequestBuilder<false>(HTTP_HEAD, url, std::ref(*this));
}

RequestBuilder<false> Client::request(http_method method, const std::string& url) {
    return RequestBuilder<false>(method, url, std::ref(*this));
}

// 全局 SSL 互斥锁：保护 libhv 的 g_ssl_ctx 线程安全问题
std::mutex& Client::global_ssl_mutex() {
    static std::mutex mutex;
    return mutex;
}

} // namespace reqhv