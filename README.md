<p align="right"><a href="README_zh.md">中文</a> | English</p>

# reqhv

A C++ HTTP client library inspired by Rust's [reqwes](https://docs.rs/reqwest/latest/reqwest/index.html), built on top of [libhv](https://github.com/ithewei/libhv).

## Features

- **Modern C++20** — Uses concepts, `std::future`, and range-based APIs
- **Builder pattern** — Fluent configuration for client and requests
- **Chainable requests** — `client.get(url).header(...).json(data).send()`
- **Synchronous by default** — `send()` blocks; `send_async()` returns `std::future<Response>`
- **Cookie management** — Built-in cookie jar with optional persistence
- **Proxy support** — HTTP/HTTPS proxy with `no_proxy` bypass list
- **TLS/mTLS** — Custom root certificates and client certificates
- **Streaming** — Chunked download support via `receive_stream()`
- **Automatic redirect** — Native libhv redirect handling with method switching

## Quick Start

### Installation

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug
```

### Your First Request

```cpp
#include <reqhv.hpp>

int main() {
    auto resp = reqhv::get("https://httpbin.org/get").send();
    std::cout << resp.status_code() << "\n";
    std::cout << resp.text() << "\n";
}
```

## Usage

### Client Configuration

```cpp
auto client = reqhv::Client::builder()
    .timeout(std::chrono::seconds(30))
    .user_agent("MyApp/1.0")
    .proxy("proxy.example.com", 8080)
    .build();
```

### GET Requests

```cpp
// Simple GET
auto resp = client.get("https://httpbin.org/get").send();

// With query parameters
auto resp = client.get("https://httpbin.org/get")
    .query("page", 1)
    .query("size", 10)
    .send();

// With custom headers
auto resp = client.get("https://httpbin.org/headers")
    .header("X-Custom-Header", "value")
    .send();
```

### POST Requests

```cpp
// POST JSON
nlohmann::json data = {{"name", "reqhv"}, {"version", "1.0"}};
auto resp = client.post("https://httpbin.org/post")
    .json(data)
    .send();

// POST form
std::map<std::string, std::string> form = {{"user", "test"}};
auto resp = client.post("https://httpbin.org/post")
    .form(form)
    .send();

// Multipart file upload
auto resp = client.post("https://httpbin.org/post")
    .multipart({{"field", "value"}})
    .file("upload", "/path/to/file.txt")
    .send();
```

### Authentication

```cpp
// Bearer token
auto resp = client.get("https://api.example.com/protected")
    .bearer_auth("my-token")
    .send();

// Basic auth
auto resp = client.post("https://httpbin.org/post")
    .basic_auth("user", "pass")
    .send();
```

### Async Requests

```cpp
// Single async request
auto future = client.get("https://httpbin.org/get").send_async();
auto resp = future.get();  // blocks

// Parallel requests
auto f1 = client.get("https://httpbin.org/get").send_async();
auto f2 = client.get("https://httpbin.org/headers").send_async();
auto resp1 = f1.get();
auto resp2 = f2.get();
```

### Streaming Downloads

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

### Error Handling

```cpp
try {
    auto resp = client.get("https://httpbin.org/status/404").send();
    resp.error_for_status();  // throws on 4xx/5xx
} catch (const reqhv::HttpException& e) {
    if (e.is_timeout()) {
        std::cerr << "Request timed out\n";
    } else if (e.is_connect()) {
        std::cerr << "Connection failed\n";
    } else if (auto code = e.status_code()) {
        std::cerr << "HTTP error: " << *code << "\n";
    }
}
```

## Global Convenience Functions

Use the global functions with a static client:

```cpp
auto resp = reqhv::get("https://httpbin.org/get").send();
auto resp = reqhv::post("https://httpbin.org/post").json({{"key", "value"}}).send();
```

## Examples

| Example | Description |
|---------|-------------|
| [basic](examples/basic/main.cpp) | GET, POST JSON, POST form, error handling |
| [response](examples/response/main.cpp) | Status codes, headers, JSON parsing, redirect URL |
| [async](examples/async/main.cpp) | Async requests, parallel fetching |
| [download](examples/download/main.cpp) | Streaming file download with progress |

## Building

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

### All Platforms with Ninja

```bash
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### With Examples and Tests

```bash
cmake .. -G "Visual Studio 17 2022" -A x64 -DBUILD_EXAMPLE=ON -DBUILD_TESTING=ON
cmake --build . --config Debug

# Run tests
ctest -C Debug --output-on-failure
```

## Architecture

```
src/
├── reqhv.hpp               # Main entry, exports convenience functions
├── config.hpp              # Configuration data struct
├── client.hpp/cpp          # Client class (wraps hv::HttpClient)
├── client_builder.hpp/cpp  # Fluent builder for Client
├── request_builder.hpp/cpp # RequestBuilder (reference_wrapper<Client>)
├── response.hpp/cpp        # Response with body parsing
├── exception.hpp/cpp       # HttpException error type
├── cookie_jar.hpp/cpp      # Cookie persistence
└── stream_chunk.hpp/cpp    # Streaming response support
```

## Dependencies

- [libhv](https://github.com/ithewei/libhv) v1.3.4 — HTTP client/server base
- [nlohmann/json](src/3rd/json.hpp) v3.10.5 — JSON parsing
- C++20 compiler (MSVC 2022+, GCC 10+, Clang 12+)