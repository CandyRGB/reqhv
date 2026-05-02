// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <functional>
#include <string>
#include <memory>
#include <future>

#include <hv/base64.h>
#include <hv/HttpClient.h>
#include <hv/httpdef.h>
#include <json.hpp>

#include "client.hpp"
#include "response.hpp"
#include "stream_chunk.hpp"

namespace reqhv {

template<bool EnableStream>
class RequestBuilder {
public:
    // 链式配置方法
    RequestBuilder& header(std::string_view key, std::string_view value);
    RequestBuilder& headers(const http_headers& headers);
    RequestBuilder& body(std::string data);
    RequestBuilder& body(const char* data, size_t len);
    RequestBuilder& json(const nlohmann::json& data);
    RequestBuilder& form(const std::map<std::string, std::string>& data);
    RequestBuilder& query(const std::string& params);
    RequestBuilder& bearer_auth(const std::string& token);
    RequestBuilder& basic_auth(const std::string& user, const std::string& pass);

    // multipart/form-data
    RequestBuilder& multipart(const std::map<std::string, std::string>& fields);
    RequestBuilder& file(const std::string& name, const std::string& filepath);

    // 同步发送，自动处理重定向
    Response send();

    // 异步发送，不支持重定向
    std::future<Response> send_async()
        requires (!EnableStream) {
        auto promise = std::make_shared<std::promise<Response>>();
        auto req = request_;
        auto& cli = client_.get().http_client();
        auto target_url = url_;
        add_cookies();
        {
            // 这里加锁可以保证每条请求会互斥地填充到 libhv 的 EventLoop 中
            std::lock_guard<std::mutex> lock(client_.get().async_send_mutex());
            cli.sendAsync(req, [this, promise](const HttpResponsePtr& resp) {
                save_cookie(resp);
                Response response(std::make_shared<HttpResponse>(*resp));
                response.set_url(url_);
                promise->set_value(std::move(response));
            });
        }
        return promise->get_future();
    }

    // 设置 http_cb 接收流式数据，配合 send() 使用
    RequestBuilder<true> receive_stream(std::shared_ptr<StreamChunk> stream_chunk) {
        request_->http_cb = stream_chunk->http_cb();
        RequestBuilder<true> new_builder(std::move(*this));
        return new_builder;
    }

    HttpRequest* raw_request() { return request_.get(); }

private:
    RequestBuilder() = delete;

    template<bool Other>
    RequestBuilder(RequestBuilder<Other>&& other) noexcept
        : request_(std::move(other.request_)),
          client_(other.client_),
          url_(std::move(other.url_)),
          redirect_count_(other.redirect_count_)
    {}

    // 供 Client 调用
    RequestBuilder(http_method method, const std::string& url, std::reference_wrapper<Client> client);
    
    std::shared_ptr<HttpRequest> request_;
    std::reference_wrapper<Client> client_;
    std::string url_;
    mutable int redirect_count_ = 0;

    Response do_send();

    void add_cookies();
    void save_cookie(const HttpResponsePtr& resp);

    friend class Client;
    friend class RequestBuilder<!EnableStream>;
};

template<bool EnableStream>
inline RequestBuilder<EnableStream>::RequestBuilder(
        http_method method,
        const std::string& url,
        std::reference_wrapper<Client> client
    ) : url_(url), client_(client) {
    request_ = std::make_shared<HttpRequest>();
    request_->method = method;
    request_->url = url;
}

template<bool EnableStream>
inline RequestBuilder<EnableStream>& RequestBuilder<EnableStream>::header(
        std::string_view key,
        std::string_view value
    ) {
    request_->headers[std::string(key)] = std::string(value);
    return *this;
}

template<bool EnableStream>
inline RequestBuilder<EnableStream>& RequestBuilder<EnableStream>::headers(
        const http_headers& headers
    ) {
    request_->headers = headers;
    return *this;
}

template<bool EnableStream>
inline RequestBuilder<EnableStream>& RequestBuilder<EnableStream>::body(
        std::string data
    ) {
    request_->body = std::move(data);
    return *this;
}

template<bool EnableStream>
inline RequestBuilder<EnableStream>& RequestBuilder<EnableStream>::body(
        const char* data, size_t len
    ) {
    request_->body.assign(data, len);
    return *this;
}

template<bool EnableStream>
inline RequestBuilder<EnableStream>& RequestBuilder<EnableStream>::json(
        const nlohmann::json& data
    ) {
    request_->json = data;
    request_->content_type = APPLICATION_JSON;
    return *this;
}

template<bool EnableStream>
inline RequestBuilder<EnableStream>& RequestBuilder<EnableStream>::form(
        const std::map<std::string, std::string>& data
    ) {
    request_->kv = data;
    request_->content_type = X_WWW_FORM_URLENCODED;
    return *this;
}

template<bool EnableStream>
inline RequestBuilder<EnableStream>& RequestBuilder<EnableStream>::query(
        const std::string& params
    ) {
    if (!url_.empty() && url_.find('?') == std::string::npos) {
        url_ += '?';
        url_ += params;
        request_->url = url_;
    }
    return *this;
}

template<bool EnableStream>
inline RequestBuilder<EnableStream>& RequestBuilder<EnableStream>::bearer_auth(
        const std::string& token
    ) {
    request_->headers["Authorization"] = "Bearer " + token;
    return *this;
}

template<bool EnableStream>
inline RequestBuilder<EnableStream>& RequestBuilder<EnableStream>::basic_auth(
        const std::string& user,
        const std::string& pass
    ) {
    auto user_pass = user + ":" + pass;
    std::string encoded = hv::Base64Encode((const unsigned char*)user_pass.data(), (unsigned int)user_pass.size());
    request_->headers["Authorization"] = "Basic " + encoded;
    return *this;
}

template<bool EnableStream>
inline RequestBuilder<EnableStream>& RequestBuilder<EnableStream>::multipart(
        const std::map<std::string, std::string>& fields
    ) {
    for (const auto& [key, value] : fields) {
        request_->SetFormData(key.c_str(), value);
    }
    request_->content_type = MULTIPART_FORM_DATA;
    return *this;
}

template<bool EnableStream>
inline RequestBuilder<EnableStream>& RequestBuilder<EnableStream>::file(
        const std::string& name,
        const std::string& filepath
    ) {
    request_->SetFormFile(name.c_str(), filepath.c_str());
    request_->content_type = MULTIPART_FORM_DATA;
    return *this;
}

template<bool EnableStream>
inline void RequestBuilder<EnableStream>::add_cookies() {
    if (!client_.get().cookie_store_enabled()) return;
    auto& jar = client_.get().cookie_jar();
    for (const auto& cookie : jar.match(url_)) {
        request_->AddCookie(cookie);
    }
}

template<bool EnableStream>
inline void RequestBuilder<EnableStream>::save_cookie(const HttpResponsePtr& resp) {
    if (!client_.get().cookie_store_enabled()) return;
    auto& jar = client_.get().cookie_jar();
    for (const auto& [key, value] : resp->headers) {
        if (key == "Set-Cookie") {
            HttpCookie cookie;
            if (cookie.parse(value)) {
                jar.add(cookie);
            }
        }
    }
}

template<bool EnableStream>
inline Response RequestBuilder<EnableStream>::do_send() {
    // 发送前：从 CookieJar 匹配 URL 对应的 cookies，添加到请求
    add_cookies();

    auto resp = std::make_shared<HttpResponse>();
    int ret = 0;
    {
        // 全局 SSL 锁：保护 libhv 的 g_ssl_ctx 线程安全问题
        std::lock_guard<std::mutex> lock(client_.get().global_ssl_mutex());
        ret = client_.get().http_client().send(request_.get(), resp.get());
    }
    if (ret != 0) {
        throw HttpException::request(ret, url_);
    }

    // 接收后：解析响应头的 Set-Cookie，存入 CookieJar
    save_cookie(resp);

    Response response(resp);
    response.set_url(url_);
    return response;
}

template<bool EnableStream>
inline Response RequestBuilder<EnableStream>::send() {
    auto resp = do_send();
    int max_redirects = client_.get().max_redirects();

    while (resp.is_redirect() && redirect_count_ < max_redirects) {
        ++redirect_count_;
        auto location = resp.header("Location");
        if (!location.has_value()) {
            break;
        }
        url_ = *location;
        request_->url = url_;
        auto status = resp.status_code();
        if ((status == 301 || status == 302 || status == 303) &&
            request_->method == HTTP_POST) {
            request_->method = HTTP_GET;
            request_->body.clear();
        }
        resp = do_send();
    }

    return resp;
}

} // namespace reqhv
