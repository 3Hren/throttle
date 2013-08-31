#include "Mocks.hpp"

TEST(Url, Class) {
    Url url;
    UNUSED(url);
}

#include "network/url_p.hpp"
TEST(Url, ParseHttpProtocol) {
    using namespace throttle::network::protocol::detail::parse;
    state_t state = parse_protocol("http://");
    EXPECT_EQ(protocol::type::Http, state.current_protocol);
    EXPECT_EQ(NoErrors, state.error_code);
    EXPECT_EQ(7, state.pos);
}

TEST(Url, ParseFtpProtocol) {
    using namespace throttle::network::protocol::detail::parse;
    state_t state = parse_protocol("ftp://");
    EXPECT_EQ(protocol::type::Ftp, state.current_protocol);
    EXPECT_EQ(NoErrors, state.error_code);
    EXPECT_EQ(6, state.pos);
}

TEST(Url, ParseWrongProtocol) {
    using namespace throttle::network::protocol::detail::parse;
    state_t state = parse_protocol("wrong:");
    EXPECT_EQ(protocol::type::Unknown, state.current_protocol);
    EXPECT_EQ(NoProtocolFound, state.error_code);
    EXPECT_EQ(0, state.pos);
}

TEST(Url, ParsePartialWrongHttpProtocol) {
    using namespace throttle::network::protocol::detail::parse;
    state_t state = parse_protocol("http:/");
    EXPECT_EQ(protocol::type::Unknown, state.current_protocol);
    EXPECT_EQ(NoProtocolFound, state.error_code);
    EXPECT_EQ(0, state.pos);
}
