#include "Mocks.hpp"

TEST(Test, Manual) {
    ev::default_loop loop;
    Ev provider{ loop };

    HttpRequestManager<Ev> manager(provider);
    std::vector<std::shared_ptr<throttle::async::Deferred<NetworkReply>>> vv;
    for (int i = 0; i < 10; ++i) {
        auto deferred = manager.get("http://httpbin.org/delay/1");
        deferred->addCallback([i](const NetworkReply &reply){
            std::cout << i << std::endl;
        });
        vv.push_back(deferred);
    }
    loop.run();
}
