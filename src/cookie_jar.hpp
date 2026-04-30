// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>

#include <hv/HttpMessage.h>

namespace reqhv {

// Cookie 存储罐，按域名分组管理 cookies。
//
// 使用方式：
//   1. Client 持有 CookieJar 实例
//   2. 发送请求前：RequestBuilder 调用 match(url) 获取匹配的 cookies
//   3. 收到响应后：RequestBuilder 调用 add(cookie) 存入 Set-Cookie
//
// 线程安全，可跨请求共享。
class CookieJar {
public:
    CookieJar() = default;

    // 添加 cookie（通常来自响应头的 Set-Cookie）。同名则覆盖。
    void add(const HttpCookie& cookie);

    // 根据 URL 匹配适用的 cookies（域名+路径匹配，RFC 6265）
    std::vector<HttpCookie> match(const std::string& url) const;

    // 移除 cookie。domain 为空则从所有域名移除。
    void remove(const std::string& name, const std::string& domain = "");

    // 从 JSON 文件加载（Client 构造时调用）
    bool load(const std::string& file_path);

    // 保存 cookies 到 JSON 文件
    bool save(const std::string& file_path) const;

    // 清空所有 cookies
    void clear();

    bool empty() const { return cookies_.empty(); }
    size_t size() const { return cookies_.size(); }

private:
    static bool domain_matches(const std::string& cookie_domain, const std::string& request_domain);
    static bool path_matches(const std::string& cookie_path, const std::string& request_path);
    static void parse_url(const std::string& url, std::string& domain, std::string& path);

    // 存储结构：domain -> [cookies]
    std::map<std::string, std::vector<HttpCookie>, std::less<>> cookies_;
    mutable std::mutex mutex_;  // 线程安全保护
};

} // namespace reqhv
