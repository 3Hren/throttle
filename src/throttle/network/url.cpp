#include "url_p.hpp"
#include "url.hpp"

using namespace throttle;
using namespace throttle::network;

Url::Url() :
    d(new detail::uri::UrlImpl)
{
}

Url::Url(const std::string &url) :
    d(new detail::uri::UrlImpl)
{
    d->parse(url);
}

Url::~Url() {
}

bool Url::isValid() const {
    return d->valid;
}
