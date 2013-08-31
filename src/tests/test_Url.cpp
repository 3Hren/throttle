#include "Mocks.hpp"

TEST(Url, Class) {
    Url url;
    UNUSED(url);
}

#include "network/url_p.hpp"
using namespace throttle::detail::uri::protocol;
using namespace throttle::detail::uri::protocol::parsers::common;
TEST(Url, ParseHttpProtocol) {
    state_t state = parse_protocol("http://");
    EXPECT_EQ(type::Http, state.current_protocol);
    EXPECT_EQ(7, state.pos);
}

TEST(Url, ParseFtpProtocol) {
    state_t state = parse_protocol("ftp://");
    EXPECT_EQ(type::Ftp, state.current_protocol);
    EXPECT_EQ(6, state.pos);
}

TEST(Url, ParseWrongProtocol) {
    state_t state = parse_protocol("wrong:");
    EXPECT_EQ(type::Unknown, state.current_protocol);
    EXPECT_EQ(0, state.pos);
}

TEST(Url, ParsePartialWrongHttpProtocol) {
    state_t state = parse_protocol("http:/");
    EXPECT_EQ(type::Unknown, state.current_protocol);
    EXPECT_EQ(0, state.pos);
}
