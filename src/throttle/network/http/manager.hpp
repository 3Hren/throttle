#pragma once

#include <list>
#include <future>
#include <memory>

#include "reply.hpp"
#include "callbacks.hpp"
#include "async/deferred.hpp"

namespace throttle { namespace network { namespace http {

class NetworkRequest;

template<typename T>
class HttpRequestManager {
    class HttpRequestManagerImpl;
    std::unique_ptr<HttpRequestManagerImpl> d;
public:
    HttpRequestManager(T &provider);
    ~HttpRequestManager();

    void get(const NetworkRequest &request, const Callbacks::OnFinished &callback) const;
    void get(const NetworkRequest &request, const Callbacks &callbacks) const;
    std::shared_ptr<throttle::async::Deferred<NetworkReply>> get(const NetworkRequest &request) const;

    void post(const NetworkRequest &request, const std::string &data, const Callbacks &callbacks) const;

    void deleteResource(const NetworkRequest &request, const Callbacks &callbacks) const;
};

} } }
