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
    EXPECT_FALSE(request.hasHeader("X-My-Header"));
    request.setHeader("X-My-Header", "123");
    EXPECT_TRUE(request.hasHeader("X-My-Header"));
    EXPECT_STREQ(request.getHeader("X-My-Header").c_str(), "123");
}
