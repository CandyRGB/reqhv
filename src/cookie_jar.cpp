// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include "cookie_jar.hpp"

#include <fstream>
#include <algorithm>

namespace reqhv {

// 从 URL 提取域名和路径。
// 实现：不依赖外部 URL 解析库，手动查找 "://"、"/"、":" 分隔符。
void CookieJar::parse_url(const std::string& url, std::string& domain, std::string& path) {
    domain.clear();
    path = "/";

    auto pos = url.find("://");
    if (pos == std::string::npos) {
        domain = url;
        return;
    }

    auto host_start = pos + 3;
    auto path_pos = url.find('/', host_start);
    if (path_pos != std::string::npos) {
        domain = url.substr(host_start, path_pos - host_start);
        path = url.substr(path_pos);
    } else {
        domain = url.substr(host_start);
    }

    // 去掉端口号
    auto colon_pos = domain.find(':');
    if (colon_pos != std::string::npos) {
        domain = domain.substr(0, colon_pos);
    }
}

// RFC 6265 域名匹配实现：
// - 精确匹配或空域名匹配
// - 以点开头 → 匹配子域名（".example.com" 匹配 "api.example.com"）
bool CookieJar::domain_matches(const std::string& cookie_domain, const std::string& request_domain) {
    if (cookie_domain.empty() || cookie_domain == request_domain) {
        return cookie_domain == request_domain;
    }
    if (!cookie_domain.empty() && cookie_domain.front() == '.') {
        auto base = cookie_domain.substr(1);
        return request_domain == base || request_domain.ends_with(base);
    }
    return false;
}

// RFC 6265 路径匹配：cookie 路径是请求路径的前缀
bool CookieJar::path_matches(const std::string& cookie_path, const std::string& request_path) {
    if (cookie_path.empty() || cookie_path == "/") return true;
    return request_path.starts_with(cookie_path);
}

// add 使用 find_if 查找同名 cookie，实现覆盖更新。
// 存储结构为 map[domain]，同名 cookie 以 name 为准进行覆盖。
void CookieJar::add(const HttpCookie& cookie) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto& domain_cookies = cookies_[cookie.domain];
    auto it = std::find_if(domain_cookies.begin(), domain_cookies.end(),
        [&](const HttpCookie& c) { return c.name == cookie.name; });
    if (it != domain_cookies.end()) {
        *it = cookie;
    } else {
        domain_cookies.push_back(cookie);
    }
}

// match 先解析 URL，再用 domain_matches + path_matches 双重过滤。
// 遍历所有 cookies 返回匹配的向量。
std::vector<HttpCookie> CookieJar::match(const std::string& url) const {
    std::string domain, path;
    parse_url(url, domain, path);

    std::vector<HttpCookie> result;
    std::lock_guard<std::mutex> lock(mutex_);

    for (const auto& [cookie_domain, domain_cookies] : cookies_) {
        if (!domain_matches(cookie_domain, domain)) continue;
        for (const auto& cookie : domain_cookies) {
            if (!path_matches(cookie.path, path)) continue;
            result.push_back(cookie);
        }
    }
    return result;
}

// remove 支持按域名精确删除，或 domain="" 时删除所有域名的同名 cookie。
void CookieJar::remove(const std::string& name, const std::string& domain) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (domain.empty()) {
        for (auto& [d, cookies] : cookies_) {
            cookies.erase(
                std::remove_if(cookies.begin(), cookies.end(),
                    [&](const HttpCookie& c) { return c.name == name; }),
                cookies.end());
        }
    } else {
        auto it = cookies_.find(domain);
        if (it != cookies_.end()) {
            it->second.erase(
                std::remove_if(it->second.begin(), it->second.end(),
                    [&](const HttpCookie& c) { return c.name == name; }),
                it->second.end());
        }
    }
}

// load/save 使用 nlohmann::json，支持序列化 HttpCookie 的所有字段。
// SameSite 字段用字符串存储（"Strict"/"Lax"/"None"）。
bool CookieJar::load(const std::string& file_path) {
    std::ifstream f(file_path);
    if (!f) return false;

    try {
        nlohmann::json j;
        f >> j;
        if (!j.is_array()) return false;

        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& item : j) {
            HttpCookie cookie;
            cookie.name = item.value("name", "");
            cookie.value = item.value("value", "");
            cookie.domain = item.value("domain", "");
            cookie.path = item.value("path", "/");
            cookie.expires = item.value("expires", "");
            cookie.max_age = item.value("max_age", 0);
            cookie.secure = item.value("secure", false);
            cookie.httponly = item.value("httponly", false);

            std::string samesite = item.value("samesite", "");
            if (samesite == "Strict") cookie.samesite = HttpCookie::Strict;
            else if (samesite == "Lax") cookie.samesite = HttpCookie::Lax;
            else if (samesite == "None") cookie.samesite = HttpCookie::None;
            else cookie.samesite = HttpCookie::Default;

            cookies_[cookie.domain].push_back(cookie);
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool CookieJar::save(const std::string& file_path) const {
    std::ofstream f(file_path);
    if (!f) return false;

    nlohmann::json j = nlohmann::json::array();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [domain, domain_cookies] : cookies_) {
            for (const auto& cookie : domain_cookies) {
                nlohmann::json item;
                item["name"] = cookie.name;
                item["value"] = cookie.value;
                item["domain"] = cookie.domain;
                item["path"] = cookie.path;
                item["expires"] = cookie.expires;
                item["max_age"] = cookie.max_age;
                item["secure"] = cookie.secure;
                item["httponly"] = cookie.httponly;

                switch (cookie.samesite) {
                    case HttpCookie::Strict: item["samesite"] = "Strict"; break;
                    case HttpCookie::Lax: item["samesite"] = "Lax"; break;
                    case HttpCookie::None: item["samesite"] = "None"; break;
                    default: break;
                }
                j.push_back(item);
            }
        }
    }

    f << j.dump(2);
    return true;
}

void CookieJar::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    cookies_.clear();
}

} // namespace reqhv
