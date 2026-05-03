// Copyright 2026 Tagca Hui
// Licensed under the MIT License

// reqhv 异步请求示例
// 演示：send_async() 返回 std::future<Response>，并行异步请求

#include <iostream>
#include <vector>
#include <chrono>

#include <reqhv.hpp>

int main() {
    std::cout << "=== reqhv async example ===\n\n";

    // ----------------------------------------
    // 1. 基础异步请求
    // ----------------------------------------
    std::cout << "1. Basic async request:\n";
    {
        auto future_resp = reqhv::get("https://httpbin.org/get").send_async();
        std::cout << "   Request sent, waiting for response...\n";

        auto resp = future_resp.get();  // 阻塞等待
        std::cout << "   Status: " << resp.status_code() << "\n";
        std::cout << "   Response: " << resp.text().substr(0, 80) << "...\n\n";
    }

    // ----------------------------------------
    // 2. 并行异步请求
    // ----------------------------------------
    std::cout << "2. Parallel async requests:\n";
    {
        auto start = std::chrono::high_resolution_clock::now();

        // 同时发起 3 个请求
        auto future1 = reqhv::get("https://httpbin.org/get").send_async();
        auto future2 = reqhv::get("https://httpbin.org/headers").send_async();
        auto future3 = reqhv::get("https://httpbin.org/uuid").send_async();

        // 等待所有响应
        auto resp1 = future1.get();
        auto resp2 = future2.get();
        auto resp3 = future3.get();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "   Request 1 status: " << resp1.status_code() << "\n";
        std::cout << "   Request 2 status: " << resp2.status_code() << "\n";
        std::cout << "   Request 3 status: " << resp3.status_code() << "\n";
        std::cout << "   Total time: " << duration << "ms (parallel, faster than sequential)\n\n";
    }

    // ----------------------------------------
    // 3. POST 异步请求
    // ----------------------------------------
    std::cout << "3. Async POST with JSON:\n";
    {
        nlohmann::json data = {
            {"async", true},
            {"message", "hello from async"}
        };

        auto future_resp = reqhv::post("https://httpbin.org/post")
                               .json(data)
                               .send_async();

        auto resp = future_resp.get();
        std::cout << "   Status: " << resp.status_code() << "\n";
        auto json_resp = resp.json();
        std::cout << "   Echo async: " << json_resp.value("/json/async", false) << "\n\n";
    }

    // ----------------------------------------
    // 4. 使用 Client 实例的异步请求
    // ----------------------------------------
    std::cout << "4. Async requests with Client instance:\n";
    {
        auto client = reqhv::Client::builder()
                          .timeout(std::chrono::seconds(10))
                          .build();

        auto future1 = client.get("https://httpbin.org/get").send_async();
        auto future2 = client.post("https://httpbin.org/post")
                              .json({{"client", "async"}})
                              .send_async();

        auto resp1 = future1.get();
        auto resp2 = future2.get();

        std::cout << "   GET status: " << resp1.status_code() << "\n";
        std::cout << "   POST status: " << resp2.status_code() << "\n\n";
    }

    // ----------------------------------------
    // 5. 异步请求超时处理
    // ----------------------------------------
    std::cout << "5. Async request with short timeout:\n";
    {
        auto client = reqhv::Client::builder()
                          .timeout(std::chrono::milliseconds(500))
                          .build();

        auto future_resp = client.get("https://httpbin.org/delay/5").send_async();

        try {
            auto resp = future_resp.get();  // 会等待超时
            std::cout << "   Status: " << resp.status_code() << "\n";
        } catch (const reqhv::HttpException& e) {
            std::cout << "   Expected timeout error: " << e.what() << "\n";
            std::cout << "   is_timeout: " << e.is_timeout() << "\n\n";
        }
    }

    std::cout << "=== Example completed ===\n";
    return 0;
}