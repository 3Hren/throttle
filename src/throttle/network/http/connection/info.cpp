#include "info.hpp"

using namespace throttle::network::http;
using namespace throttle::network::http::connection;

Info::Info(const NetworkRequest &request, const Callbacks &callbacks):
    easy(curl_easy_init()),
    request(request),
    callbacks(callbacks)
{
}

Info::~Info()
{
    curl_easy_cleanup(easy);
}
