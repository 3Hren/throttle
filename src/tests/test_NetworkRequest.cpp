#include "Mocks.hpp"

TEST(NetworkRequest, Class) {
    NetworkRequest request;
    UNUSED(request);
}

TEST(NetworkRequest, UrlGetSet) {
    NetworkRequest request;
    request.setUrl("http://localhost");
    EXPECT_STREQ("http://localhost", request.getUrl().c_str());
}

TEST(NetworkRequest, HeaderGetSet) {
    NetworkRequest request;
    EXPECT_FALSE(request.headers().has("X-My-Header"));
    request.headers().set("X-My-Header", "123");
    EXPECT_TRUE(request.headers().has("X-My-Header"));
    EXPECT_STREQ(request.headers().get("X-My-Header").c_str(), "123");
}

TEST(NetworkRequest, ContentTypeHeaderGetSet) {
    NetworkRequest request;
    EXPECT_FALSE(request.headers().has<Header::ContentType>());
    request.headers().set<Header::ContentType>("text/html");
    EXPECT_TRUE(request.headers().has<Header::ContentType>());
    EXPECT_STREQ(request.headers().get<Header::ContentType>().c_str(), "text/html");
}

TEST(NetworkRequest, ContentLengthHeaderGetSet) {
    NetworkRequest request;
    EXPECT_FALSE(request.headers().has<Header::ContentLength>());
    request.headers().set<Header::ContentLength>(1024);
    EXPECT_TRUE(request.headers().has<Header::ContentLength>());
    EXPECT_EQ(request.headers().get<Header::ContentLength>(), 1024);
}
