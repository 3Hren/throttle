#include <ev++.h>

#include <boost/format.hpp>
#include <boost/asio.hpp>

#include "Mocks.hpp"

TEST(HttpRequestManager, Class) {
    ev::default_loop loop;
    EvProvider provider{ loop };
    HttpRequestManager<EvProvider> manager(provider);
    UNUSED(manager);
}

TEST(HttpRequestManager, SimpleRequestToApache) {
    ev::default_loop loop;
    EvProvider provider{ loop };

    bool completed = false;
    HttpRequestManager<EvProvider> manager(provider);
    manager.get("http://localhost", [&completed](HttpReply&& reply){
        EXPECT_STREQ("<html><body><h1>It works!</h1></body></html>", reply.body.c_str());
        completed = true;
    });
    loop.run();
    EXPECT_TRUE(completed);
}

TEST(HttpRequestManager, Manual) {
    ev::default_loop loop;
    EvProvider provider{ loop };

    HttpRequestManager<EvProvider> manager(provider);
    manager.get("http://www.google.cn", [](HttpReply&& reply){
        std::cout << "=== Response received:" << std::endl;
        for (auto header : reply.headers)
            std::cout << boost::format("%s : %s") % header.name % header.value << std::endl;
        std::cout << boost::format("Body: %s") % reply.body << std::endl;
    });
//    for (int i = 0; i < 5; ++i)
//    manager.get("http://www.google.ru", [i](const HttpReply &reply){
//        std::cout << "=== Response received:" << i << std::endl;
//    });
    loop.run();
}
