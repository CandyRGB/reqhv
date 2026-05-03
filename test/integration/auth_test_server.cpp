// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "auth_test_server.hpp"
#include <hv/base64.h>
#include <iostream>

namespace reqhv {
namespace test {

void AuthTestServer::start() {
    // Bearer token 验证
    router_.GET("/bearer", [](HttpRequest* req, HttpResponse* resp) {
        auto auth_header = req->GetHeader("Authorization");
        if (auth_header.empty()) {
            resp->status_code = HTTP_STATUS_UNAUTHORIZED;
            return resp->String(R"({"error": "missing authorization"})");
        }
        if (auth_header.find("Bearer ") != 0) {
            resp->status_code = HTTP_STATUS_UNAUTHORIZED;
            return resp->String(R"({"error": "invalid bearer token"})");
        }
        std::string token = auth_header.substr(7);
        return resp->Json(R"({"token": ")" + token + R"("})");
    });

    // Basic auth 验证 - 固定用户密码验证
    router_.GET("/basic-auth", [](HttpRequest* req, HttpResponse* resp) {
        auto auth_header = req->GetHeader("Authorization");
        if (auth_header.empty()) {
            resp->status_code = HTTP_STATUS_UNAUTHORIZED;
            return resp->String(R"({"error": "missing basic auth"})");
        }
        if (auth_header.find("Basic ") != 0) {
            resp->status_code = HTTP_STATUS_UNAUTHORIZED;
            return resp->String(R"({"error": "invalid basic auth format"})");
        }

        std::string encoded = auth_header.substr(6);
        std::string expected = hv::Base64Encode((const unsigned char*)"user:passwd", 11);
        if (encoded == expected) {
            return resp->Json(R"({"status": "ok", "user": "user"})");
        }
        resp->status_code = HTTP_STATUS_UNAUTHORIZED;
        return resp->Json(R"({"error": "invalid credentials"})");
    });

    server_.registerHttpService(&router_);
    server_.setPort(port_);
    server_.start();
}

void AuthTestServer::stop() {
    server_.stop();
}

} // namespace test
} // namespace reqhv