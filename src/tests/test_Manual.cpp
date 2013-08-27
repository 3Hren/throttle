#include "Mocks.hpp"

TEST(Test, Manual) {
    ev::default_loop loop;
    Ev provider{ loop };

    HttpRequestManager<Ev> manager(provider);
    for (int i = 0; i < 10; ++i)
        manager.get("http://httpbin.org/delay/1", [i](HttpReply&& reply){
            std::cout << "=== Response received: " << i << std::endl;
        });
    loop.run();
}
