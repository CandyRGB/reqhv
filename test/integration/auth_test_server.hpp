// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <hv/HttpServer.h>
#include <hv/HttpMessage.h>

namespace reqhv {
namespace test {

class AuthTestServer {
public:
    AuthTestServer(int port) : port_(port) {}
    void start();
    void stop();

private:
    int port_;
    HttpService router_;
    hv::HttpServer server_;
};

} // namespace test
} // namespace reqhv