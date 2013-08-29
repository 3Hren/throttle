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

TEST(HeaderList, Class) {
    HeaderList headers;
    UNUSED(headers);
}

TEST(HeaderList, GetSet) {
    HeaderList headers;
    EXPECT_FALSE(headers.has("X-My-Header"));
    headers.set("X-My-Header", "123");
    EXPECT_TRUE(headers.has("X-My-Header"));
    EXPECT_EQ(headers.get("X-My-Header"), "123");
}

TEST(HeaderList, ContentTypeHeaderGetSet) {
    HeaderList headers;
    EXPECT_FALSE(headers.has<Header::ContentType>());
    headers.set<Header::ContentType>("text/html");
    EXPECT_TRUE(headers.has<Header::ContentType>());
    EXPECT_EQ(headers.get<Header::ContentType>(), "text/html");
}

TEST(HeaderList, ContentLengthHeaderGetSet) {
    HeaderList headers;
    EXPECT_FALSE(headers.has<Header::ContentLength>());
    headers.set<Header::ContentLength>(1024);
    EXPECT_TRUE(headers.has<Header::ContentLength>());
    EXPECT_EQ(headers.get<Header::ContentLength>(), 1024);
}

TEST(HeaderList, OverridesHeaderOnSet) {
    HeaderList headers;
    headers.set({"Header1", "initial"});
    headers.set({"Header1", "overriden"});
    EXPECT_EQ(1, headers.size());
    EXPECT_EQ("overriden", headers.get("Header1"));
}
