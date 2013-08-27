#include "Mocks.hpp"

TEST(Test, Manual) {
    ev::default_loop loop;
    Ev provider{ loop };

    HttpRequestManager<Ev> manager(provider);
    std::vector<std::future<NetworkReply>> futures;
    for (int i = 0; i < 10; ++i) {
        auto f = manager.get("http://httpbin.org/delay/1");
        futures.push_back(std::move(f));
    }
    loop.run();
}
