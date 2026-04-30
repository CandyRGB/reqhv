// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "request_builder.hpp"
#include "client.hpp"
#include <hv/base64.h>

namespace reqhv {

RequestBuilder::RequestBuilder(http_method method, const std::string& url, std::reference_wrapper<Client> client)
    : url_(url), client_(client) {
    request_ = std::make_shared<HttpRequest>();
    request_->method = method;
    request_->url = url;
}

RequestBuilder& RequestBuilder::header(std::string_view key, std::string_view value) {
    request_->headers[std::string(key)] = std::string(value);
    return *this;
}

RequestBuilder& RequestBuilder::headers(const http_headers& headers) {
    request_->headers = headers;
    return *this;
}

RequestBuilder& RequestBuilder::body(std::string data) {
    request_->body = std::move(data);
    return *this;
}

RequestBuilder& RequestBuilder::body(const char* data, size_t len) {
    request_->body.assign(data, len);
    return *this;
}

RequestBuilder& RequestBuilder::json(const nlohmann::json& data) {
    request_->json = data;
    request_->content_type = APPLICATION_JSON;
    return *this;
}

RequestBuilder& RequestBuilder::form(const std::map<std::string, std::string>& data) {
    request_->kv = data;
    request_->content_type = X_WWW_FORM_URLENCODED;
    return *this;
}

RequestBuilder& RequestBuilder::query(const std::string& params) {
    if (!url_.empty() && url_.find('?') == std::string::npos) {
        url_ += '?';
        url_ += params;
        request_->url = url_;
    }
    return *this;
}

RequestBuilder& RequestBuilder::bearer_auth(const std::string& token) {
    request_->headers["Authorization"] = "Bearer " + token;
    return *this;
}

RequestBuilder& RequestBuilder::basic_auth(const std::string& user, const std::string& pass) {
    auto user_pass = user + ":" + pass;
    std::string encoded = hv::Base64Encode((const unsigned char*)user_pass.data(), (unsigned int)user_pass.size());
    request_->headers["Authorization"] = "Basic " + encoded;
    return *this;
}

Response RequestBuilder::do_send() {
    // 发送前：从 CookieJar 匹配 URL 对应的 cookies，添加到请求
    if (client_.get().cookie_store_enabled()) {
        auto& jar = client_.get().cookie_jar();
        for (const auto& cookie : jar.match(url_)) {
            request_->AddCookie(cookie);
        }
    }

    auto resp = std::make_shared<HttpResponse>();
    int ret = client_.get().http_client().send(request_.get(), resp.get());
    if (ret != 0) {
        throw HttpException::request(ret, url_);
    }

    // 接收后：解析响应头的 Set-Cookie，存入 CookieJar
    if (client_.get().cookie_store_enabled()) {
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

    Response response(resp);
    response.set_url(url_);
    return response;
}

Response RequestBuilder::send() {
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

std::future<Response> RequestBuilder::send_async() {
    auto promise = std::make_shared<std::promise<Response>>();
    auto req = request_;
    auto& cli = client_.get().http_client();
    auto target_url = url_;
    {
        // 这里加锁可以保证每条请求会互斥地填充到 libhv 的 EventLoop 中
        std::lock_guard<std::mutex> lock(async_send_mutex_);
        cli.sendAsync(req, [promise, target_url](const HttpResponsePtr& resp) {
            Response response(std::make_shared<HttpResponse>(*resp));
            response.set_url(target_url);
            promise->set_value(std::move(response));
        });
    }
    return promise->get_future();
}

} // namespace reqhv
