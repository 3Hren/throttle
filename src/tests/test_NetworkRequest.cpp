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
