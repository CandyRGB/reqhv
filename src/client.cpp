// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "request_builder.hpp"
#include "client.hpp"

namespace reqhv {

Client Client::create() {
    return Client();
}

ClientBuilder Client::builder() {
    return ClientBuilder();
}

// 从 Builder 配置构造 Client
Client::Client(ClientBuilder& builder)
    : timeout_(builder.timeout_)
    , user_agent_(builder.user_agent_)
    , default_headers_(builder.default_headers_)
    , max_redirects_(builder.max_redirects_) {}

RequestBuilder Client::get(const std::string& url) {
    return RequestBuilder(HTTP_GET, url);
}

RequestBuilder Client::post(const std::string& url) {
    return RequestBuilder(HTTP_POST, url);
}

RequestBuilder Client::put(const std::string& url) {
    return RequestBuilder(HTTP_PUT, url);
}

RequestBuilder Client::patch(const std::string& url) {
    return RequestBuilder(HTTP_PATCH, url);
}

RequestBuilder Client::delete_(const std::string& url) {
    return RequestBuilder(HTTP_DELETE, url);
}

RequestBuilder Client::head(const std::string& url) {
    return RequestBuilder(HTTP_HEAD, url);
}

RequestBuilder Client::request(http_method method, const std::string& url) {
    return RequestBuilder(method, url);
}

} // namespace reqhv