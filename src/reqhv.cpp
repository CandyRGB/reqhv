// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "reqhv.hpp"

namespace reqhv {

// 全局默认 Client 实例，用于顶层便捷函数
static Client s_global_client = Client::create();

RequestBuilder get(const std::string& url) {
    return s_global_client.get(url);
}

RequestBuilder post(const std::string& url) {
    return s_global_client.post(url);
}

RequestBuilder put(const std::string& url) {
    return s_global_client.put(url);
}

RequestBuilder patch(const std::string& url) {
    return s_global_client.patch(url);
}

RequestBuilder delete_(const std::string& url) {
    return s_global_client.delete_(url);
}

RequestBuilder head(const std::string& url) {
    return s_global_client.head(url);
}

RequestBuilder request(Method method, const std::string& url) {
    return s_global_client.request(method, url);
}

} // namespace reqhv