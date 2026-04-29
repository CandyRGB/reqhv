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
LIBHV_SOURCE_DIR="E:/Projects/libhv" cmake ..
```

## Architecture

reqhv is a C++ HTTP client library inspired by Rust's reqwest, built on top of libhv 1.3.4.

### Core Types
- `Client` - Main HTTP client, created via `Client::builder().build()`
- `ClientBuilder` - Fluent builder for Client configuration
- `RequestBuilder` - Builds requests with headers, body, auth, etc.
- `Response` - HTTP response with status, body parsing, headers
- `HttpException` - Unified error type with status codes

### File Structure
```
src/
├── reqhv.hpp           # Main entry, exports convenience functions (get/post/etc.)
├── client.hpp/cpp      # Client class
├── client_builder.hpp/cpp  # ClientBuilder class
├── request_builder.hpp/cpp # RequestBuilder class
├── response.hpp/cpp    # Response class
├── exception.hpp/cpp   # HttpException class
└── 3rd/json.hpp        # nlohmann/json header-only library
```

### Key Design Patterns
- **Builder pattern**: `Client::builder()->timeout(10s)->user_agent("...")->build()`
- **Chainable requests**: `client.get(url).header(...).json(data).send()`
- **Synchronous by default**: `send()` blocks; `send_async()` returns `std::future<Response>`
- **Redirect handling**: Automatic redirect with 301/302/303 method switching
- **`delete_` with underscore**: The C++ keyword `delete` cannot be used, so the method is named `delete_`

### libhv Integration
- Uses `hv_static` (static library) to avoid DLL dependency
- `http_client_send()` for synchronous requests
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
    return RequestBuilder(HTTP_PATCH, url);
}
```

### Add a new builder option
1. Add method declaration in `client_builder.hpp`
2. Add member variable in private section
3. Implement in `client_builder.cpp`
4. Apply in `Client::Client(ClientBuilder&)` constructor

## Notes
- Uses C++20 standard but `CMAKE_CXX_EXTENSIONS OFF` (no compiler extensions)
- IDE include errors are normal before build; resolve after first cmake generation
- hv.dll is built but not needed when linking against hv_static