// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <hv/HttpServer.h>
#include <hv/HttpMessage.h>
#include <hv/json.hpp>

namespace reqhv {
namespace test {

class CookieTestServer {
public:
    CookieTestServer(int port) : port_(port) {}
    void start();
    void stop();

private:
    int port_;
    HttpService router_;
    hv::HttpServer server_;
};

} // namespace test
} // namespace reqhv