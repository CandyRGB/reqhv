// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "cookie_test_server.hpp"

namespace reqhv {
namespace test {

void CookieTestServer::start() {
    router_.GET("/set-cookie", [](HttpRequest* req, HttpResponse* resp) {
        resp->SetHeader("Set-Cookie", "test_cookie=test_value; Path=/");
        resp->SetHeader("Set-Cookie", "another_cookie=abc123; Path=/; Max-Age=3600");
        return resp->String("{\"status\": \"cookie set\"}");
    });

    router_.GET("/get-cookies", [](HttpRequest* req, HttpResponse* resp) {
        return resp->String("{\"cookies\": {\"test_cookie\": \"test_value\"}}");
    });

    router_.GET("/echo", [](HttpRequest* req, HttpResponse* resp) {
        return resp->String("{\"method\": \"GET\"}");
    });

    server_.registerHttpService(&router_);
    server_.setPort(port_);
    server_.start();
}

void CookieTestServer::stop() {
    server_.stop();
}

} // namespace test
} // namespace reqhv