#include <ev++.h>

#include <boost/format.hpp>
#include <boost/asio.hpp>

#include <rapidjson/document.h>

#include "Mocks.hpp"

TEST(HttpRequestManager, Class) {
    ev::default_loop loop;
    Ev provider{ loop };
    HttpRequestManager<Ev> manager(provider);
    UNUSED(manager);
}

TEST(HttpRequestManager, SimpleRequestToApache) {
    ev::default_loop loop;
    Ev provider{ loop };

    bool completed = false;
    HttpRequestManager<Ev> manager(provider);
    manager.get("http://localhost", [&completed](HttpReply&& reply){
        EXPECT_STREQ("<html><body><h1>It works!</h1></body></html>", reply.body.c_str());
        completed = true;
    });
    loop.run();
    EXPECT_TRUE(completed);
}

TEST(HttpRequestManager, SimpleRequestToApacheUsingNetworkRequest) {
    ev::default_loop loop;
    Ev provider{ loop };

    bool completed = false;
    HttpRequestManager<Ev> manager(provider);
    NetworkRequest request("http://localhost");
    manager.get(request, [&completed](HttpReply&& reply){
        EXPECT_STREQ("<html><body><h1>It works!</h1></body></html>", reply.body.c_str());
        completed = true;
    });
    loop.run();
    EXPECT_TRUE(completed);
}

TEST(HttpRequestManager, PackHeaders) {
    ev::default_loop loop;
    Ev provider{ loop };

    bool completed = false;
    HttpRequestManager<Ev> manager(provider);
    NetworkRequest request("http://httpbin.org/headers");
    request.setHeader("X-My-Header", "123");
    manager.get(request, [&completed](HttpReply&& reply){
        EXPECT_TRUE(reply.body.find("\"X-My-Header\": \"123\"") != std::string::npos);
        completed = true;
    });
    EXPECT_FALSE(request.hasHeader("1"));
    loop.run();
    EXPECT_TRUE(completed);
}

TEST(HttpRequestManager, UnpackHeaders) {
    ev::default_loop loop;
    Ev provider{ loop };

    bool completed = false;
    HttpRequestManager<Ev> manager(provider);
    NetworkRequest request("http://httpbin.org/response-headers?X-My-Header=123");
    manager.get(request, [&completed](HttpReply&& reply){
        EXPECT_TRUE(reply.hasHeader("X-My-Header"));
        EXPECT_STREQ("123", reply.getHeader("X-My-Header").c_str());
        completed = true;
    });
    loop.run();
    EXPECT_TRUE(completed);
}

TEST(HttpRequestManager, Manual) {
    ev::default_loop loop;
    Ev provider{ loop };

    HttpRequestManager<Ev> manager(provider);
    manager.get("http://www.google.cn", [](HttpReply&& reply){
        std::cout << "=== Response received:" << std::endl;
        for (auto header : reply.headers)
            std::cout << boost::format("%s : %s") % header.name % header.value << std::endl;
        std::cout << boost::format("Body: %s") % reply.body << std::endl;
    });
    loop.run();
}
