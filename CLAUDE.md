# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

```shell
# Configure and build
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug

# Run tests
ctest -C Debug --output-on-failure

# Run a single test
ctest -C Debug --output-on-failure -R "TestName"

# Build with local libhv (if not using FetchContent)
cmake ..

# Build with examples and tests
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DBUILD_EXAMPLE=ON -DBUILD_TESTING=ON
cmake --build . --config Debug
```

## Architecture

reqhv is a C++ HTTP client library inspired by Rust's reqwest, built on top of libhv 1.3.4.

### Core Types
- `Client` - Main HTTP client, created via `Client::builder().build()` or `Client::create()` for defaults
- `ClientBuilder` - Fluent builder for Client configuration
- `Config` - Configuration data struct (timeout, user_agent, headers, proxy, TLS, etc.)
- `RequestBuilder<false>` - Builds regular requests with headers, body, auth; `RequestBuilder<true>` for streaming
- `Response` - HTTP response with status, body parsing, headers
- `HttpException` - Unified error type with status codes

### File Structure
```
src/
├── reqhv.hpp           # Main entry, exports convenience functions (get/post/etc.)
├── config.hpp          # Config struct for all client settings
├── client.hpp/cpp      # Client class (uses hv::HttpClient RAII)
├── client_builder.hpp/cpp  # ClientBuilder class
├── request_builder.hpp/cpp # RequestBuilder class (uses reference_wrapper<Client>)
├── response.hpp/cpp    # Response class
├── exception.hpp/cpp   # HttpException class
├── cookie_jar.hpp/cpp  # Cookie persistence
├── stream_chunk.hpp/cpp # Streaming response support
└── 3rd/json.hpp        # nlohmann/json header-only library
```

### Examples
```
examples/
├── basic/      # Basic GET/POST requests
├── response/   # Response parsing
├── async/      # Async requests with futures
└── download/   # Stream download with progress
```

### Key Design Patterns
- **Builder pattern**: `Client::builder().timeout(10s).user_agent("...")->build()`
- **Chainable requests**: `client.get(url).header(...).json(data).send()`
- **Synchronous by default**: `send()` blocks; `send_async()` returns `std::future<Response>`
- **Redirect handling**: Automatic redirect with 301/302/303 method switching
- **`delete_` with underscore**: The C++ keyword `delete` cannot be used, so the method is named `delete_`
- **Streaming responses**: `RequestBuilder::receive_stream()` returns `StreamChunk` for chunked/stream download
- **Config as data**: All client settings stored in `Config` struct, shared between ClientBuilder and Client
- **RAII via hv::HttpClient**: `Client` wraps `hv::HttpClient` (libhv's C++ RAII wrapper) instead of raw `http_client_t*`

### libhv Integration
- Uses `hv::HttpClient` (RAII C++ wrapper) instead of raw C API
- `client.http_client().send(req, resp)` for synchronous requests
- `http_headers` (typedef for `std::map<std::string, std::string, hv::StringCaseLess>`)
- `http_method` enum for HTTP verbs (HTTP_GET, HTTP_POST, etc.)

### Dependencies
- libhv 1.3.4 (via FetchContent or local)
- nlohmann/json 3.10.5 (header-only in src/3rd/)

## Common Tasks

### Add a new HTTP method to Client
```cpp
// In client.hpp, add method declaration
RequestBuilder patch(const std::string& url);

// In client.cpp, implement
RequestBuilder Client::patch(const std::string& url) {
    return RequestBuilder(HTTP_PATCH, url, std::ref(*this));
}
```

### Add a new builder option
1. Add member to `Config` struct in `config.hpp`
2. Add builder method in `ClientBuilder::config_.<option> = value` style
3. Apply in `Client::Client(const Config&)` constructor

## Notes
- Uses C++20 standard
- IDE include errors are normal before build; resolve after first cmake generation
- `RequestBuilder` stores `std::reference_wrapper<Client>`, use `client_.get()` to access
- `Client` holds `Config config_` and `hv::HttpClient http_client_`
- Global convenience functions (`reqhv::get()` etc.) use static `Client` singleton