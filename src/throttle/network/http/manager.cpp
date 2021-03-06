#include <functional>

#include "utils.hpp"
#include "event/watcher.hpp"
#include "request.hpp"
#include "manager.hpp"
#include "manager_p.hpp"
#include "connection/info.hpp"

using namespace std::placeholders;

using namespace throttle::event;
using namespace throttle::network::http;

struct ReplyRedirectAction {
    std::shared_ptr<throttle::async::Deferred<NetworkReply>> deferred;
    void operator ()(NetworkReply &&reply) {
        deferred->trigger(std::move(reply));
    }
};

template<typename T>
HttpRequestManager<T>::HttpRequestManager(T &provider) :
    d(new HttpRequestManagerImpl(provider))
{
}

template<typename T>
HttpRequestManager<T>::~HttpRequestManager()
{
}

template<typename T>
void HttpRequestManager<T>::get(const NetworkRequest &request, const Callbacks::OnFinished &callback) const
{
    get(request, Callbacks(callback));
}

template<typename T>
void HttpRequestManager<T>::get(const NetworkRequest &request, const Callbacks &callbacks) const
{
    d->get(request, callbacks);
}

template<typename T>
DeferredNetworkReply HttpRequestManager<T>::get(const NetworkRequest &request) const
{
    DeferredNetworkReply deferred(std::make_shared<throttle::async::Deferred<NetworkReply>>());
    ReplyRedirectAction action{ deferred };
    get(request, action);
    return deferred;
}

template<typename T>
void HttpRequestManager<T>::post(const NetworkRequest &request, const std::string &data, const Callbacks &callbacks) const
{
    d->post(request, data, callbacks);
}

template<typename T>
void HttpRequestManager<T>::deleteResource(const NetworkRequest &request, const Callbacks &callbacks) const
{
    d->deleteResource(request, callbacks);
}

#include "event/provider/ev.hpp"
template class throttle::network::http::HttpRequestManager<throttle::event::provider::Ev>;
