// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "reqhv.hpp"
#include "client_builder.hpp"

namespace reqhv {

// 全局默认 Client 实例，用于顶层便捷函数
static Client s_default_client = [] {
    return Client::builder()
        .timeout(std::chrono::seconds(30))
        .user_agent("reqhv/1.0")
        .build();
}();

RequestBuilder get(const std::string& url) {
    return s_default_client.get(url);
}

RequestBuilder post(const std::string& url) {
    return s_default_client.post(url);
}

RequestBuilder put(const std::string& url) {
    return s_default_client.put(url);
}

RequestBuilder patch(const std::string& url) {
    return s_default_client.patch(url);
}

RequestBuilder delete_(const std::string& url) {
    return s_default_client.delete_(url);
}

RequestBuilder head(const std::string& url) {
    return s_default_client.head(url);
}

RequestBuilder request(Method method, const std::string& url) {
    return s_default_client.request(method, url);
}

} // namespace reqhv