// Copyright 2026 Tagca Hui
// Licensed under the MIT License

// reqhv 文件下载示例
// 演示：使用 StreamChunk 流式下载文件到磁盘

#include <iostream>
#include <fstream>
#include <chrono>

#include <reqhv.hpp>

using namespace std::chrono;

int main() {
    std::cout << "=== reqhv file download example ===\n\n";

    // 创建 Client 实例
    auto client = reqhv::Client::builder()
                      .timeout(60s)
                      .build();

    // 创建 stream_chunk 用于流式接收
    auto stream_chunk = std::make_shared<reqhv::StreamChunk>();

    // 发起请求，设置 http_cb 流式接收
    std::cout << "Downloading...\n";
    auto resp = client.get("https://speed.cloudflare.com/__down?bytes=4096000")
                    .receive_stream(stream_chunk)
                    .send();

    // 边接收边写入文件
    std::string filename = "downloaded_file.bin";
    std::ofstream outfile(filename, std::ios::binary);

    auto start = std::chrono::high_resolution_clock::now();
    size_t total_bytes = 0;

    while (auto chunk = stream_chunk->next()) {
        outfile.write(chunk->data(), chunk->size());
        total_bytes += chunk->size();
    }
    outfile.close();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    double speed = total_bytes * 1000.0 / duration / 1024 / 1024;

    std::cout << "   Status: " << resp.status_code() << "\n";
    std::cout << "   Downloaded: " << total_bytes << " bytes\n";
    std::cout << "   Time: " << duration << " ms\n";
    std::cout << "   Speed: " << speed << " MB/s\n\n";

    std::cout << "=== Example completed ===\n";
    return 0;
}