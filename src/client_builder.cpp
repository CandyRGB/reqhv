// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "client.hpp"
#include "client_builder.hpp"

namespace reqhv {

ClientBuilder::ClientBuilder() = default;

ClientBuilder& ClientBuilder::timeout(std::chrono::milliseconds ms) {
    timeout_ = ms;
    return *this;
}

ClientBuilder& ClientBuilder::connect_timeout(std::chrono::milliseconds ms) {
    connect_timeout_ = ms;
    return *this;
}

ClientBuilder& ClientBuilder::user_agent(std::string_view ua) {
    user_agent_ = std::string(ua);
    return *this;
}

ClientBuilder& ClientBuilder::default_headers(const http_headers& headers) {
    default_headers_ = headers;
    return *this;
}

ClientBuilder& ClientBuilder::cookie_store(bool enable) {
    cookie_store_ = enable;
    return *this;
}

ClientBuilder& ClientBuilder::redirect(int max_redirects) {
    max_redirects_ = max_redirects;
    return *this;
}

ClientBuilder& ClientBuilder::gzip(bool enable) {
    gzip_ = enable;
    return *this;
}

ClientBuilder& ClientBuilder::danger_accept_invalid_certs(bool accept) {
    danger_accept_invalid_certs_ = accept;
    return *this;
}

Client ClientBuilder::build() {
    return Client(*this);
}

} // namespace reqhv