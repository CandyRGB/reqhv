// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "reqhv.hpp"
#include "client_builder.hpp"

namespace reqhv {

RequestBuilder get(const std::string& url) {
    return Client::create().get(url);
}

RequestBuilder post(const std::string& url) {
    return Client::create().post(url);
}

RequestBuilder put(const std::string& url) {
    return Client::create().put(url);
}

RequestBuilder patch(const std::string& url) {
    return Client::create().patch(url);
}

RequestBuilder delete_(const std::string& url) {
    return Client::create().delete_(url);
}

RequestBuilder head(const std::string& url) {
    return Client::create().head(url);
}

RequestBuilder request(Method method, const std::string& url) {
    return Client::create().request(method, url);
}

} // namespace reqhv