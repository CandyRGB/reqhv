// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "client.hpp"
#include "client_builder.hpp"
#include <hv/HttpClient.h>

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

Client ClientBuilder::build() {
    return Client(std::move(config_));
}

} // namespace reqhv