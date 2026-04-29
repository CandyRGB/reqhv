// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "exception.hpp"

namespace reqhv {

HttpException HttpException::timeout(const std::string& url) {
    return HttpException("request timeout", ErrorKind::Timeout);
}

HttpException HttpException::connect(const std::string& url) {
    return HttpException("connection failed", ErrorKind::Connect);
}

HttpException HttpException::request(int status_code, const std::string& url) {
    return HttpException("http request failed", ErrorKind::Request, status_code);
}

HttpException HttpException::body(const std::string& msg) {
    return HttpException(msg, ErrorKind::Body);
}

HttpException HttpException::decode(const std::string& msg) {
    return HttpException(msg, ErrorKind::Decode);
}

} // namespace reqhv