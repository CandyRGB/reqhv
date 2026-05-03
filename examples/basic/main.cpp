// Copyright 2026 Tagca Hui
// Licensed under the MIT License

// reqhv 基础用法示例
// 演示：GET 请求、POST JSON、POST Form、响应处理、错误处理

#include <iostream>

#include <reqhv.hpp>

int main() {
    std::cout << "=== reqhv basic example ===\n\n";

    // ----------------------------------------
    // 1. 简单 GET 请求
    // ----------------------------------------
    std::cout << "1. Simple GET request:\n";
    try {
        auto resp = reqhv::get("https://httpbin.org/get").send();
        std::cout << "   Status: " << resp.status_code() << "\n";
        std::cout << "   Response: " << resp.text().substr(0, 100) << "...\n\n";
    } catch (const reqhv::HttpException& e) {
        std::cerr << "   Error: " << e.what() << "\n\n";
    }

    // ----------------------------------------
    // 2. POST JSON 请求
    // ----------------------------------------
    std::cout << "2. POST JSON request:\n";
    try {
        nlohmann::json data = {
            {"name", "reqhv"},
            {"version", "1.0.0"},
            {"features", {"http", "json", "tls"}}
        };
        auto resp = reqhv::post("https://httpbin.org/post")
                        .json(data)
                        .send();
        std::cout << "   Status: " << resp.status_code() << "\n";

        auto json_resp = resp.json();
        std::cout << "   JSON response origin: " << json_resp.value("origin", "unknown") << "\n\n";
    } catch (const reqhv::HttpException& e) {
        std::cerr << "   Error: " << e.what() << "\n\n";
    }

    // ----------------------------------------
    // 3. POST Form 请求
    // ----------------------------------------
    std::cout << "3. POST form request:\n";
    try {
        std::map<std::string, std::string> form_data = {
            {"username", "testuser"},
            {"password", "testpass"}
        };
        auto resp = reqhv::post("https://httpbin.org/post")
                        .form(form_data)
                        .send();
        std::cout << "   Status: " << resp.status_code() << "\n";
        std::cout << "   Response: " << resp.text().substr(0, 100) << "...\n\n";
    } catch (const reqhv::HttpException& e) {
        std::cerr << "   Error: " << e.what() << "\n\n";
    }

    // ----------------------------------------
    // 4. 带 Header 的请求
    // ----------------------------------------
    std::cout << "4. GET with custom headers:\n";
    try {
        auto resp = reqhv::get("https://httpbin.org/headers")
                        .header("X-Custom-Header", "Hello")
                        .header("Accept", "application/json")
                        .send();
        std::cout << "   Status: " << resp.status_code() << "\n";
        std::cout << "   Response: " << resp.text() << "\n\n";
    } catch (const reqhv::HttpException& e) {
        std::cerr << "   Error: " << e.what() << "\n\n";
    }

    // ----------------------------------------
    // 5. 错误处理示例 (404)
    // ----------------------------------------
    std::cout << "5. Error handling (404 Not Found):\n";
    try {
        auto resp = reqhv::get("https://httpbin.org/status/404").send();
        resp.error_for_status();  // 4xx 会抛出异常
        std::cout << "   Status: " << resp.status_code() << "\n";
    } catch (const reqhv::HttpException& e) {
        std::cout << "   Expected error: " << e.what() << "\n";
        std::cout << "   Error kind: " << static_cast<int>(e.kind()) << "\n";
        std::cout << "   Status code: " << e.status_code().value_or(0) << "\n\n";
    }

    std::cout << "=== Example completed ===\n";
    return 0;
}