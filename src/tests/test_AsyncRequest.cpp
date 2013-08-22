#include <boost/format.hpp>
#include <boost/asio.hpp>

#include "Mocks.hpp"

TEST(HttpRequestManager, Class) {
    boost::asio::io_service io_service;
    HttpRequestManager manager(io_service);
    UNUSED(manager);
}

TEST(HttpRequestManager, SimpleHttpRequest) {
    boost::asio::io_service io_service;

    HttpRequestManager manager(io_service);
    manager.get("http://127.0.0.1", [](const HttpReply &reply){
        std::cout << boost::format("Reply: %s") % reply.body << std::endl;
    });
    io_service.run();
}
