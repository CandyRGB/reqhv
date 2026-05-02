// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "reqhv.hpp"

#include "client_builder.hpp"

namespace reqhv {

RequestBuilder<false> get(const std::string& url) {
    return Client::create().get(url);
}

RequestBuilder<false> post(const std::string& url) {
    return Client::create().post(url);
}

RequestBuilder<false> put(const std::string& url) {
    return Client::create().put(url);
}

RequestBuilder<false> patch(const std::string& url) {
    return Client::create().patch(url);
}

RequestBuilder<false> delete_(const std::string& url) {
    return Client::create().delete_(url);
}

RequestBuilder<false> head(const std::string& url) {
    return Client::create().head(url);
}

RequestBuilder<false> request(Method method, const std::string& url) {
    return Client::create().request(method, url);
}

} // namespace reqhv