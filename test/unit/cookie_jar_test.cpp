// Copyright 2026 Tagca Hui
// Licensed under the MIT License

#include <gtest/gtest.h>
#include "cookie_jar.hpp"

using namespace reqhv;

TEST(CookieJarAdd, AddNewCookie) {
    CookieJar jar;
    HttpCookie cookie;
    cookie.name = "session";
    cookie.value = "abc123";
    cookie.domain = "example.com";
    cookie.path = "/";

    jar.add(cookie);
    EXPECT_EQ(jar.size(), 1);
}

TEST(CookieJarAdd, UpdateExistingCookieByName) {
    CookieJar jar;
    HttpCookie cookie1;
    cookie1.name = "session";
    cookie1.value = "old";
    cookie1.domain = "example.com";
    cookie1.path = "/";

    HttpCookie cookie2;
    cookie2.name = "session";
    cookie2.value = "new";
    cookie2.domain = "example.com";
    cookie2.path = "/";

    jar.add(cookie1);
    jar.add(cookie2);
    EXPECT_EQ(jar.size(), 1);

    auto matched = jar.match("http://example.com/");
    EXPECT_EQ(matched.size(), 1);
    EXPECT_EQ(matched[0].value, "new");
}

TEST(CookieJarAdd, MultipleDomains) {
    CookieJar jar;
    HttpCookie c1;
    c1.name = "a"; c1.domain = "example.com"; c1.path = "/";
    jar.add(c1);

    HttpCookie c2;
    c2.name = "b"; c2.domain = "other.com"; c2.path = "/";
    jar.add(c2);

    EXPECT_EQ(jar.size(), 2);
}

TEST(CookieJarMatch, ExactDomainMatch) {
    CookieJar jar;
    HttpCookie cookie;
    cookie.name = "session";
    cookie.value = "abc";
    cookie.domain = "example.com";
    cookie.path = "/";

    jar.add(cookie);

    auto matched = jar.match("http://example.com/");
    EXPECT_EQ(matched.size(), 1);
    EXPECT_EQ(matched[0].value, "abc");
}

TEST(CookieJarMatch, SubdomainMatchWithLeadingDot) {
    CookieJar jar;
    HttpCookie cookie;
    cookie.name = "session";
    cookie.value = "abc";
    cookie.domain = ".example.com";
    cookie.path = "/";

    jar.add(cookie);

    auto matched = jar.match("http://api.example.com/");
    EXPECT_EQ(matched.size(), 1);
    EXPECT_EQ(matched[0].value, "abc");

    matched = jar.match("http://sub.api.example.com/");
    EXPECT_EQ(matched.size(), 1);
}

TEST(CookieJarMatch, PathPrefixMatching) {
    CookieJar jar;
    HttpCookie cookie;
    cookie.name = "session";
    cookie.value = "abc";
    cookie.domain = "example.com";
    cookie.path = "/api";

    jar.add(cookie);

    auto matched = jar.match("http://example.com/api");
    EXPECT_EQ(matched.size(), 1);

    matched = jar.match("http://example.com/api/users");
    EXPECT_EQ(matched.size(), 1);

    matched = jar.match("http://example.com/other");
    EXPECT_TRUE(matched.empty());
}

TEST(CookieJarMatch, RootPathMatchesAll) {
    CookieJar jar;
    HttpCookie cookie;
    cookie.name = "session";
    cookie.value = "abc";
    cookie.domain = "example.com";
    cookie.path = "/";

    jar.add(cookie);

    auto matched = jar.match("http://example.com/any/path/here");
    EXPECT_EQ(matched.size(), 1);
}

TEST(CookieJarMatch, NoMatchForDifferentDomain) {
    CookieJar jar;
    HttpCookie cookie;
    cookie.name = "session";
    cookie.value = "abc";
    cookie.domain = "example.com";
    cookie.path = "/";

    jar.add(cookie);

    auto matched = jar.match("http://other.com/");
    EXPECT_TRUE(matched.empty());
}

TEST(CookieJarRemove, RemoveByNameAcrossAllDomains) {
    CookieJar jar;
    HttpCookie c1;
    c1.name = "a"; c1.domain = "x.com"; c1.path = "/";
    jar.add(c1);

    HttpCookie c2;
    c2.name = "a"; c2.domain = "y.com"; c2.path = "/";
    jar.add(c2);

    jar.remove("a");
    EXPECT_TRUE(jar.empty());
}

TEST(CookieJarRemove, RemoveByNameInSpecificDomain) {
    CookieJar jar;
    HttpCookie c1;
    c1.name = "a"; c1.domain = "x.com"; c1.path = "/";
    jar.add(c1);

    HttpCookie c2;
    c2.name = "b"; c2.domain = "x.com"; c2.path = "/";
    jar.add(c2);

    jar.remove("a", "x.com");
    EXPECT_EQ(jar.size(), 1);
    auto matched = jar.match("http://x.com/");
    EXPECT_EQ(matched.size(), 1);
    EXPECT_EQ(matched[0].name, "b");
}

TEST(CookieJarRemove, RemoveNonExistent) {
    CookieJar jar;
    HttpCookie c;
    c.name = "existing"; c.domain = "example.com"; c.path = "/";
    jar.add(c);

    jar.remove("nonexistent");
    EXPECT_EQ(jar.size(), 1);
}

TEST(CookieJarClear, ClearAll) {
    CookieJar jar;
    HttpCookie c;
    c.name = "a"; c.domain = "example.com"; c.path = "/";
    jar.add(c);
    EXPECT_FALSE(jar.empty());

    jar.clear();
    EXPECT_TRUE(jar.empty());
}

TEST(CookieJarClear, ClearEmpty) {
    CookieJar jar;
    jar.clear();
    EXPECT_TRUE(jar.empty());
}

TEST(CookieJarSaveLoad, SaveAndLoad) {
    CookieJar jar;
    HttpCookie c;
    c.name = "session";
    c.value = "abc123";
    c.domain = "example.com";
    c.path = "/api";
    c.secure = true;
    c.httponly = true;
    jar.add(c);

    const char* path = "test_cookies.json";
    EXPECT_TRUE(jar.save(path));

    CookieJar jar2;
    EXPECT_TRUE(jar2.load(path));
    EXPECT_EQ(jar2.size(), 1);

    auto matched = jar2.match("http://example.com/api");
    EXPECT_EQ(matched.size(), 1);
    EXPECT_EQ(matched[0].name, "session");
    EXPECT_EQ(matched[0].value, "abc123");
    EXPECT_TRUE(matched[0].secure);
    EXPECT_TRUE(matched[0].httponly);
}

TEST(CookieJarSaveLoad, LoadNonExistentFile) {
    CookieJar jar;
    EXPECT_FALSE(jar.load("nonexistent_file_12345.json"));
}

TEST(CookieJarSaveLoad, SaveEmptyJar) {
    CookieJar jar;
    const char* path = "test_empty_cookies.json";
    EXPECT_TRUE(jar.save(path));

    CookieJar jar2;
    EXPECT_TRUE(jar2.load(path));
    EXPECT_TRUE(jar2.empty());
}

TEST(CookieJarSaveLoad, SaveMultipleCookies) {
    CookieJar jar;
    for (int i = 0; i < 5; ++i) {
        HttpCookie c;
        c.name = "cookie" + std::to_string(i);
        c.value = "value" + std::to_string(i);
        c.domain = "example.com";
        c.path = "/";
        jar.add(c);
    }

    const char* path = "test_multi_cookies.json";
    EXPECT_TRUE(jar.save(path));

    CookieJar jar2;
    EXPECT_TRUE(jar2.load(path));
    // size() returns number of domains (keys), not total cookies
    EXPECT_EQ(jar2.size(), 1);

    auto matched = jar2.match("http://example.com/");
    EXPECT_EQ(matched.size(), 5);
}