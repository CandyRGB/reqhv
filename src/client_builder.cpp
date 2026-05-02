// Copyright 2026 Tagca Hui
// Licensed under the MIT License


#include "client_builder.hpp"

#include <hv/HttpClient.h>

#include "client.hpp"

namespace reqhv {

ClientBuilder& ClientBuilder::timeout(std::chrono::milliseconds ms) {
    config_.timeout = ms;
    return *this;
}

ClientBuilder& ClientBuilder::connect_timeout(std::chrono::milliseconds ms) {
    config_.connect_timeout = ms;
    return *this;
}

ClientBuilder& ClientBuilder::user_agent(std::string_view ua) {
    config_.user_agent = std::string(ua);
    return *this;
}

ClientBuilder& ClientBuilder::default_headers(const http_headers& headers) {
    config_.default_headers = headers;
    return *this;
}

ClientBuilder& ClientBuilder::cookie_store(bool enable) {
    config_.cookie_store = enable;
    return *this;
}

ClientBuilder& ClientBuilder::cookie_path(const std::string& path) {
    config_.cookie_file_path = path;
    return *this;
}

ClientBuilder& ClientBuilder::redirect(int max_redirects) {
    config_.max_redirects = max_redirects;
    return *this;
}

ClientBuilder& ClientBuilder::gzip(bool enable) {
    config_.gzip = enable;
    return *this;
}

ClientBuilder& ClientBuilder::danger_accept_invalid_certs(bool accept) {
    config_.danger_accept_invalid_certs = accept;
    return *this;
}

ClientBuilder& ClientBuilder::proxy(const std::string& host, int port) {
    config_.proxy_host = host;
    config_.proxy_port = port;
    return *this;
}

ClientBuilder& ClientBuilder::https_proxy(const std::string& host, int port) {
    config_.https_proxy_host = host;
    config_.https_proxy_port = port;
    return *this;
}

ClientBuilder& ClientBuilder::no_proxy(const std::vector<std::string>& hosts) {
    config_.no_proxy = hosts;
    return *this;
}

ClientBuilder& ClientBuilder::no_proxy(std::initializer_list<std::string> hosts) {
    config_.no_proxy = std::vector<std::string>(hosts);
    return *this;
}

ClientBuilder& ClientBuilder::add_root_certificate(const std::string& cert_pem) {
    config_.root_cert_pem = cert_pem;
    return *this;
}

ClientBuilder& ClientBuilder::identity(const std::string& cert_pem, const std::string& key_pem) {
    config_.client_cert_pem = cert_pem;
    config_.client_key_pem = key_pem;
    return *this;
}

Client ClientBuilder::build() {
    return Client(std::move(config_));
}

} // namespace reqhv