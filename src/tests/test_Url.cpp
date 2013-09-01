#include "Mocks.hpp"

namespace throttle { namespace network {

TEST(Url, Class) {
    Url url;
    UNUSED(url);
}

} }

#include <boost/spirit/include/qi.hpp>
template<typename Iterator>
// RFC 3986 - http://tools.ietf.org/html/rfc3986#page-49
struct uri_grammar : public boost::spirit::qi::grammar<Iterator> {
    uri_grammar() : uri_grammar::base_type(uri) {
        namespace qi = boost::spirit::qi;
        uri             %= ((scheme >> ':') >> hier_part >> -('?' >> query) >> -('#' >> fragment));
        hier_part       %=
                ((("//" >> userinfo >> '@') | "//") >> host >> -(':' >> port) >> path_abempty)
                | path_absolute
                | path_rootless
                | path_empty;
        userinfo        %= qi::raw[*(unreserved | pct_encoded | sub_delims | qi::char_(":"))];
        scheme          %= qi::raw[qi::alpha >> *(qi::alnum | qi::char_("+-."))];
        host            %= qi::raw[ipv4_address | reg_name];
        port            %= qi::raw[qi::ushort_ | qi::eps];
        reg_name        %= qi::raw[*(unreserved | pct_encoded | sub_delims)];
        unreserved      %= qi::alnum | qi::char_("-._~");
        pct_encoded     %= qi::char_("%") >> qi::repeat(2)[qi::xdigit];
        sub_delims      %= qi::char_("!$&'()*+,;=");

        ipv4_address    %= qi::raw[dec_octet >> qi::repeat(3)[qi::lit('.') >> dec_octet]];
        dec_octet       %= !(qi::lit('0') >> qi::digit) >> qi::raw[qi::uint_parser<std::uint8_t, 10, 1, 3>()];

        path_abempty    %= *('/' >> segment);
        path_absolute   %= '/' >> -(segment_nz >> *('/' >> segment));
        path_rootless   %= segment_nz >> *('/' >> segment);
        path_empty      %= qi::eps;

        segment         %= qi::raw[*pchar];
        segment_nz      %= qi::raw[+pchar];

        pchar           %= qi::raw[unreserved | pct_encoded | sub_delims | qi::char_(":@")];
        query           %= *(pchar | qi::char_("/?"));
        fragment        %= *(pchar | qi::char_("/?"));
    }

    boost::spirit::qi::rule<Iterator> uri;

    boost::spirit::qi::rule<Iterator> hier_part;
    boost::spirit::qi::rule<Iterator> userinfo;
    boost::spirit::qi::rule<Iterator> path_abempty, path_absolute, path_rootless, path_empty;
    boost::spirit::qi::rule<Iterator> segment, segment_nz;
    boost::spirit::qi::rule<Iterator> pchar;

    boost::spirit::qi::rule<Iterator> scheme;
    boost::spirit::qi::rule<Iterator> query;
    boost::spirit::qi::rule<Iterator> fragment;

    boost::spirit::qi::rule<Iterator> host, port;
    boost::spirit::qi::rule<Iterator> reg_name;
    boost::spirit::qi::rule<Iterator> unreserved;
    boost::spirit::qi::rule<Iterator> pct_encoded;
    boost::spirit::qi::rule<Iterator> sub_delims;
    boost::spirit::qi::rule<Iterator> ipv4_address;
    boost::spirit::qi::rule<Iterator> dec_octet;
};

namespace detail {
bool parse_uri(std::string uri) {
    static uri_grammar<std::string::iterator> grammar;
    std::string::iterator first = uri.begin();
    std::string::iterator last = uri.end();
    const bool valid = boost::spirit::qi::parse(first, last, grammar);
    return valid && (first == last);
}
}

TEST(uri_grammar, SuccessDomain) {
    EXPECT_TRUE(detail::parse_uri("yandex.ru"));
    EXPECT_TRUE(detail::parse_uri("yandex-team.ru"));

    EXPECT_TRUE(detail::parse_uri("www.yandex.ru"));
    EXPECT_TRUE(detail::parse_uri("www.google.ru"));

}

TEST(uri_grammar, FailedDomain) {
    EXPECT_FALSE(detail::parse_uri("yandex#team.ru"));
}

TEST(uri_grammar, IpV4Success) {
    EXPECT_TRUE(detail::parse_uri("192.168.1.0"));
    EXPECT_TRUE(detail::parse_uri("192.168.1.1"));
    EXPECT_TRUE(detail::parse_uri("192.168.1.255"));
}

TEST(uri_grammar, IpV4Failed) {
    EXPECT_FALSE(detail::parse_uri("192.168.1.256"));
    EXPECT_FALSE(detail::parse_uri("192.256.1.1"));
    EXPECT_FALSE(detail::parse_uri("192.168.256.1"));
    EXPECT_FALSE(detail::parse_uri("256.168.1.1"));
}
