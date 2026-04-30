// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include "client.hpp"
#include "client_builder.hpp"
#include "request_builder.hpp"
#include "response.hpp"
#include "exception.hpp"

using Method = http_method;

namespace reqhv {

RequestBuilder get(const std::string& url);
RequestBuilder post(const std::string& url);
RequestBuilder put(const std::string& url);
RequestBuilder patch(const std::string& url);
RequestBuilder delete_(const std::string& url);
RequestBuilder head(const std::string& url);
RequestBuilder request(Method method, const std::string& url);

} // namespace reqhv
