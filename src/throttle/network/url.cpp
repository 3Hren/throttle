#include "url_p.hpp"
#include "url.hpp"

using namespace throttle::network;

Url::Url() :
    d(new UrlImpl)
{
}

Url::Url(const std::string &url) :
    d(new UrlImpl)
{
    d->parse(url);
}

Url::~Url() {
}

bool Url::isValid() const {
    return d->valid;
}
