// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "redirect_test_server.hpp"

namespace reqhv {
namespace test {

RedirectTestServer::RedirectTestServer(int port) : port_(port) {}

void RedirectTestServer::start() {
    router_.GET("/redirect301", [](HttpRequest* req, HttpResponse* resp) {
        return resp->Redirect("/final", HTTP_STATUS_MOVED_PERMANENTLY);
    });

    router_.GET("/redirect302", [](HttpRequest* req, HttpResponse* resp) {
        return resp->Redirect("/final", HTTP_STATUS_FOUND);
    });

    router_.POST("/redirect303", [](HttpRequest* req, HttpResponse* resp) {
        return resp->Redirect("/final", HTTP_STATUS_SEE_OTHER);
    });

    router_.GET("/multi-redirect", [](HttpRequest* req, HttpResponse* resp) {
        return resp->Redirect("/step2", HTTP_STATUS_FOUND);
    });

    router_.GET("/step2", [](HttpRequest* req, HttpResponse* resp) {
        return resp->Redirect("/final", HTTP_STATUS_FOUND);
    });

    router_.GET("/final", [](HttpRequest* req, HttpResponse* resp) {
        return resp->Json(R"({"status": "ok", "path": "/final"})");
    });

    router_.POST("/final", [](HttpRequest* req, HttpResponse* resp) {
        return resp->Json(R"({"status": "ok", "path": "/final"})");
    });

    router_.GET("/limit-redirects", [](HttpRequest* req, HttpResponse* resp) {
        auto it = req->query_params.find("remaining");
        if (it != req->query_params.end()) {
            int remaining = std::atoi(it->second.c_str());
            if (remaining > 0) {
                return resp->Redirect("/limit-redirects?remaining=" + std::to_string(remaining - 1), HTTP_STATUS_FOUND);
            }
        }
        return resp->Json(R"({"status": "ok", "path": "/limit-redirects"})");
    });

    server_.registerHttpService(&router_);
    server_.setPort(port_);
    server_.start();
}

void RedirectTestServer::stop() {
    server_.stop();
}

} // namespace test
} // namespace reqhv