<p align="right">中文 | <a href="README.md">English</a></p>

# reqhv

受 Rust [reqwest](https://docs.rs/reqwest/latest/reqwest/index.html) 启发，基于 [libhv](https://github.com/ithewei/libhv) 构建的 C++ HTTP 客户端库。

## 特性

- **现代 C++20** — 使用 concepts、`std::future` 和 range-based APIs
- **构建器模式** — 流畅配置客户端和请求
- **链式调用** — `client.get(url).header(...).json(data).send()`
- **同步优先** — `send()` 阻塞；`send_async()` 返回 `std::future<Response>`
- **Cookie 管理** — 内置 cookie jar，支持持久化
- **代理支持** — HTTP/HTTPS 代理，支持 `no_proxy` 跳过列表
- **TLS/mTLS** — 自定义根证书和客户端证书
- **流式响应** — 通过 `receive_stream()` 支持分块下载
- **自动重定向** — libhv 原生重定向处理，自动切换请求方法

## 快速开始

### 安装

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug
```

### 第一个请求

```cpp
#include <reqhv.hpp>

int main() {
    auto resp = reqhv::get("https://httpbin.org/get").send();
    std::cout << resp.status_code() << "\n";
    std::cout << resp.text() << "\n";
}
```

## 使用

### 客户端配置

```cpp
auto client = reqhv::Client::builder()
    .timeout(std::chrono::seconds(30))
    .user_agent("MyApp/1.0")
    .proxy("proxy.example.com", 8080)
    .build();
```

### GET 请求

```cpp
// 简单 GET
auto resp = client.get("https://httpbin.org/get").send();

// 带查询参数
auto resp = client.get("https://httpbin.org/get")
    .query("page", 1)
    .query("size", 10)
    .send();

// 带自定义请求头
auto resp = client.get("https://httpbin.org/headers")
    .header("X-Custom-Header", "value")
    .send();
```

### POST 请求

```cpp
// POST JSON
nlohmann::json data = {{"name", "reqhv"}, {"version", "1.0"}};
auto resp = client.post("https://httpbin.org/post")
    .json(data)
    .send();

// POST 表单
std::map<std::string, std::string> form = {{"user", "test"}};
auto resp = client.post("https://httpbin.org/post")
    .form(form)
    .send();

// Multipart 文件上传
auto resp = client.post("https://httpbin.org/post")
    .multipart({{"field", "value"}})
    .file("upload", "/path/to/file.txt")
    .send();
```

### 认证

```cpp
// Bearer token
auto resp = client.get("https://api.example.com/protected")
    .bearer_auth("my-token")
    .send();

// Basic 认证
auto resp = client.post("https://httpbin.org/post")
    .basic_auth("user", "pass")
    .send();
```

### 异步请求

```cpp
// 单个异步请求
auto future = client.get("https://httpbin.org/get").send_async();
auto resp = future.get();  // 阻塞等待

// 并行请求
auto f1 = client.get("https://httpbin.org/get").send_async();
auto f2 = client.get("https://httpbin.org/headers").send_async();
auto resp1 = f1.get();
auto resp2 = f2.get();
```

### 流式下载

```cpp
auto stream = std::make_shared<reqhv::StreamChunk>();
auto resp = client.get("https://example.com/large-file.zip")
    .receive_stream(stream)
    .send();

std::ofstream out("output.bin", std::ios::binary);
while (auto chunk = stream->next()) {
    out.write(chunk->data(), chunk->size());
}
```

### 错误处理

```cpp
try {
    auto resp = client.get("https://httpbin.org/status/404").send();
    resp.error_for_status();  // 4xx/5xx 会抛出异常
} catch (const reqhv::HttpException& e) {
    if (e.is_timeout()) {
        std::cerr << "请求超时\n";
    } else if (e.is_connect()) {
        std::cerr << "连接失败\n";
    } else if (auto code = e.status_code()) {
        std::cerr << "HTTP 错误: " << *code << "\n";
    }
}
```

## 全局便捷函数

可以使用全局便捷函数和静态客户端：

```cpp
auto resp = reqhv::get("https://httpbin.org/get").send();
auto resp = reqhv::post("https://httpbin.org/post").json({{"key", "value"}}).send();
```

## 示例

| 示例 | 说明 |
|------|------|
| [basic](examples/basic/main.cpp) | GET、POST JSON、POST 表单、错误处理 |
| [response](examples/response/main.cpp) | 状态码、响应头、JSON 解析、重定向 URL |
| [async](examples/async/main.cpp) | 异步请求、并行获取 |
| [download](examples/download/main.cpp) | 流式文件下载，支持进度显示 |

## 构建

### Windows (MSVC)

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug
```

### Linux / macOS

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### 全平台 Ninja

```bash
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### 构建示例和测试

```bash
cmake .. -G "Visual Studio 17 2022" -A x64 -DBUILD_EXAMPLE=ON -DBUILD_TESTING=ON
cmake --build . --config Debug

# 运行测试
ctest -C Debug --output-on-failure
```

## 架构

```
src/
├── reqhv.hpp               # 主入口，导出便捷函数
├── config.hpp              # 配置数据结构
├── client.hpp/cpp          # Client 类（封装 hv::HttpClient）
├── client_builder.hpp/cpp  # Client 构建器
├── request_builder.hpp/cpp # RequestBuilder（reference_wrapper<Client>）
├── response.hpp/cpp        # Response，响应体解析
├── exception.hpp/cpp       # HttpException 异常类型
├── cookie_jar.hpp/cpp      # Cookie 持久化
└── stream_chunk.hpp/cpp    # 流式响应支持
```

## 依赖

- [libhv](https://github.com/ithewei/libhv) v1.3.4 — HTTP 客户端/服务端底层
- [nlohmann/json](src/3rd/json.hpp) v3.10.5 — JSON 解析
- C++20 编译器 (MSVC 2022+, GCC 10+, Clang 12+)