// Copyright 2026 Tagca Hui
// Licensed under the MIT License

// reqhv 响应处理示例
// 演示：状态码检查、响应体解析、错误处理、重定向处理

#include <iostream>

#include <reqhv.hpp>

int main() {
    std::cout << "=== reqhv response handling example ===\n\n";

    // ----------------------------------------
    // 1. 状态码检查
    // ----------------------------------------
    std::cout << "1. Status code checking:\n";
    try {
        auto resp = reqhv::get("https://httpbin.org/status/200").send();
        std::cout << "   200 OK: is_success=" << resp.is_success()
                  << ", is_redirect=" << resp.is_redirect()
                  << ", is_client_error=" << resp.is_client_error()
                  << ", is_server_error=" << resp.is_server_error() << "\n";

        auto resp404 = reqhv::get("https://httpbin.org/status/404").send();
        std::cout << "   404 Not Found: is_success=" << resp404.is_success()
                  << ", is_client_error=" << resp404.is_client_error() << "\n";

        auto resp500 = reqhv::get("https://httpbin.org/status/500").send();
        std::cout << "   500 Server Error: is_success=" << resp500.is_success()
                  << ", is_server_error=" << resp500.is_server_error() << "\n\n";
    } catch (const reqhv::HttpException& e) {
        std::cerr << "   Error: " << e.what() << "\n\n";
    }

    // ----------------------------------------
    // 2. 响应体解析 - text()
    // ----------------------------------------
    std::cout << "2. Response body - text():\n";
    try {
        auto resp = reqhv::get("https://httpbin.org/robots.txt").send();
        std::cout << "   Status: " << resp.status_code() << "\n";
        std::cout << "   Body: " << resp.text() << "\n\n";
    } catch (const reqhv::HttpException& e) {
        std::cerr << "   Error: " << e.what() << "\n\n";
    }

    // ----------------------------------------
    // 3. 响应体解析 - json()
    // ----------------------------------------
    std::cout << "3. Response body - json():\n";
    try {
        auto resp = reqhv::get("https://httpbin.org/json").send();
        std::cout << "   Status: " << resp.status_code() << "\n";

        auto j = resp.json();
        std::cout << "   JSON structure:\n";
        std::cout << "     slideshow.author: " << j.value("/slideshow/author"_json_pointer, "unknown") << "\n";
        std::cout << "     slideshow.title: " << j.value("/slideshow/title"_json_pointer, "unknown") << "\n\n";
    } catch (const reqhv::HttpException& e) {
        std::cerr << "   Error: " << e.what() << "\n\n";
    }

    // ----------------------------------------
    // 4. Header 访问
    // ----------------------------------------
    std::cout << "4. Header access:\n";
    try {
        auto resp = reqhv::get("https://httpbin.org/headers").send();
        std::cout << "   Status: " << resp.status_code() << "\n";

        auto content_type = resp.header("Content-Type");
        std::cout << "   Content-Type: " << (content_type ? *content_type : "not found") << "\n";

        auto server = resp.header("Server");
        std::cout << "   Server: " << (server ? *server : "not found") << "\n\n";
    } catch (const reqhv::HttpException& e) {
        std::cerr << "   Error: " << e.what() << "\n\n";
    }

    // ----------------------------------------
    // 5. error_for_status() 错误处理
    // ----------------------------------------
    std::cout << "5. error_for_status() - throws on 4xx/5xx:\n";
    try {
        auto resp = reqhv::get("https://httpbin.org/status/400").send();
        resp.error_for_status();  // 4xx 会抛出异常
        std::cout << "   Should not reach here\n";
    } catch (const reqhv::HttpException& e) {
        std::cout << "   Caught expected error:\n";
        std::cout << "     what(): " << e.what() << "\n";
        std::cout << "     kind(): " << static_cast<int>(e.kind()) << " (3=Request)\n";
        std::cout << "     status_code(): " << e.status_code().value_or(0) << "\n";
        std::cout << "     url(): " << e.url() << "\n\n";
    }

    // ----------------------------------------
    // 6. 重定向后 URL 获取
    // ----------------------------------------
    std::cout << "6. Final URL after redirect:\n";
    try {
        auto resp = reqhv::get("https://httpbin.org/redirect-to?url=https://example.com").send();
        std::cout << "   Final URL: " << resp.url() << "\n";
        std::cout << "   Status: " << resp.status_code() << "\n\n";
    } catch (const reqhv::HttpException& e) {
        std::cerr << "   Error: " << e.what() << "\n\n";
    }

    // ----------------------------------------
    // 7. HttpException 错误类型判断
    // ----------------------------------------
    std::cout << "7. HttpException error kinds:\n";
    try {
        // Timeout 错误 - 通过 ClientBuilder 设置超时
        auto client = reqhv::Client::builder()
                          .timeout(std::chrono::milliseconds(100))
                          .build();
        auto resp = client.get("https://httpbin.org/delay/10").send();
    } catch (const reqhv::HttpException& e) {
        std::cout << "   Timeout error: is_timeout=" << e.is_timeout() << "\n";
        std::cout << "     what(): " << e.what() << "\n\n";
    }

    try {
        // 连接错误 (无效端口)
        auto resp = reqhv::get("https://httpbin.org:99999/status/200").send();
    } catch (const reqhv::HttpException& e) {
        std::cout << "   Connect error: is_connect=" << e.is_connect() << "\n";
        std::cout << "     what(): " << e.what() << "\n\n";
    }

    std::cout << "=== Example completed ===\n";
    return 0;
}