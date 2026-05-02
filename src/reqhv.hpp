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

RequestBuilder<false> get(const std::string& url);
RequestBuilder<false> post(const std::string& url);
RequestBuilder<false> put(const std::string& url);
RequestBuilder<false> patch(const std::string& url);
RequestBuilder<false> delete_(const std::string& url);
RequestBuilder<false> head(const std::string& url);
RequestBuilder<false> request(Method method, const std::string& url);

} // namespace reqhv
