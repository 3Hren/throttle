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
    manager.get("http://localhost", [&completed](NetworkReply&& reply){
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
    manager.get(request, [&completed](NetworkReply&& reply){
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
    request.headers().set("X-My-Header", "123");
    manager.get(request, [&completed](NetworkReply&& reply){
        EXPECT_TRUE(reply.body.find("\"X-My-Header\": \"123\"") != std::string::npos);
        completed = true;
    });
    EXPECT_FALSE(request.headers().has("1"));
    loop.run();
    EXPECT_TRUE(completed);
}

TEST(HttpRequestManager, UnpackHeaders) {
    ev::default_loop loop;
    Ev provider{ loop };

    bool completed = false;
    HttpRequestManager<Ev> manager(provider);
    NetworkRequest request("http://httpbin.org/response-headers?X-My-Header=123");
    manager.get(request, [&completed](NetworkReply&& reply){
        EXPECT_TRUE(reply.headers().has("X-My-Header"));
        EXPECT_STREQ("123", reply.headers().get("X-My-Header").c_str());
        completed = true;
    });
    loop.run();
    EXPECT_TRUE(completed);
}

TEST(HttpRequestManager, Post) {
    ev::default_loop loop;
    Ev provider{ loop };

    const std::string data("{\"data\": 123}");
    bool completed = false;
    HttpRequestManager<Ev> manager(provider);
    NetworkRequest request("http://httpbin.org/post");
    manager.post(request, data, {[&completed, &data](NetworkReply&& reply){
        rapidjson::Document d;
        std::cout << reply.body << std::endl;
        d.Parse<0>(reply.body.c_str());
        ASSERT_FALSE(d.HasParseError());
        EXPECT_TRUE(d.HasMember("data"));
        std::string actual = d["data"].GetString();
        EXPECT_STREQ(data.c_str(), actual.c_str());
        completed = true;
    }});
    loop.run();
    EXPECT_TRUE(completed);
}

TEST(HttpRequestManager, Delete) {
    ev::default_loop loop;
    Ev provider{ loop };

    const std::string data("{\"data\": 123}");
    bool completed = false;
    HttpRequestManager<Ev> manager(provider);
    NetworkRequest request("http://httpbin.org/delete");
    manager.deleteResource(request, {[&completed, &data](NetworkReply&& reply){
        rapidjson::Document d;
        std::cout << reply.body << std::endl;
        d.Parse<0>(reply.body.c_str());
        ASSERT_FALSE(d.HasParseError());
        completed = true;
    }});
    loop.run();
    EXPECT_TRUE(completed);
}

TEST(HttpRequestManager, Manual) {
    ev::default_loop loop;
    Ev provider{ loop };

    HttpRequestManager<Ev> manager(provider);
    manager.get("http://www.google.cn", [](NetworkReply&& reply){
        std::cout << "=== Response received:" << std::endl;
        for (auto header : reply.headers().all())
            std::cout << boost::format("%s : %s") % header.name % header.value << std::endl;
        std::cout << boost::format("Body: %s") % reply.body << std::endl;
    });
    loop.run();
}
