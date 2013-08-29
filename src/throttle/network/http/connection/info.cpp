#include <boost/algorithm/string.hpp>

#include "info.hpp"

using namespace throttle::network::http;
using namespace throttle::network::http::connection;

Info::Info(const NetworkRequest &request, const Callbacks &callbacks):
    easy(curl_easy_init()),
    request(request),
    callbacks(callbacks)
{
    curl_easy_setopt(easy, CURLOPT_URL, request.getUrl().c_str());
    curl_easy_setopt(easy, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(easy, CURLOPT_NOPROGRESS, 1L);

    struct curl_slist *curl_headers = packHeaders(request.headers().all());
    curl_easy_setopt(easy, CURLOPT_HTTPHEADER, curl_headers);

    curl_easy_setopt(easy, CURLOPT_HEADERFUNCTION, onHeader);
    curl_easy_setopt(easy, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(easy, CURLOPT_WRITEFUNCTION, onBody);
    curl_easy_setopt(easy, CURLOPT_WRITEDATA, this);

    curl_easy_setopt(easy, CURLOPT_PRIVATE, this);
}

Info::~Info()
{
    curl_easy_cleanup(easy);
}

void Info::send(CURLM *multi) const {
    CURLMcode code = curl_multi_add_handle(multi, easy);
    if (code != CURLM_OK) {
        LOG_DEBUG("sending request failed - %d", code);
        if (callbacks.onFinished) {
            callbacks.onFinished(NetworkReply());
        }
    }
}

size_t Info::onBody(char *data, size_t size, size_t nmemb, Info *info) {
    LOG_DEBUG("size: %d, nmemb: %d", size, nmemb);
    const size_t written = size * nmemb;
    info->bodyStream.write(data, written);
    if (info->callbacks.onBody) {
        info->callbacks.onBody(data, written);
    }
    return written;
}

size_t Info::onHeader(char *data, size_t size, size_t nmemb, Info *info) {
    LOG_DEBUG("size: %d, nmemb: %d", size, nmemb);
    const size_t written = size * nmemb;

    char *lf;
    char *end = data + written;
    char *colon = std::find(data, end, ':');

    if (colon != end) {
        const std::string field = helpers::trimmed(data, colon);
        std::string value;

        data = colon + 1;
        do {
            lf = std::find(data, end, '\n');
            if (!value.empty())
                value += ' ';

            value += helpers::trimmed(data, lf);
            data = lf;
        } while (data < end && (*(data + 1) == ' ' || *(data + 1) == '\t'));

        Header header(field, value);
        if (info->callbacks.onHeader) {
            info->callbacks.onHeader(header);
        }
        info->headers.push_back(std::move(header));
    }

    return written;
}

curl_slist *Info::packHeaders(const HeaderList::Container::Type &headers) const {
    struct curl_slist *curl_headers = nullptr;
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        addHeader(it->name, it->value, &curl_headers);
    }
    return curl_headers;
}

void Info::addHeader(const std::string &name, const std::string &value, struct curl_slist **curl_headers) const {
    std::string rawHeader;
    rawHeader.reserve(name.size() + 2 + value.size());
    rawHeader += name;
    rawHeader += ": ";
    rawHeader += value;
    *curl_headers = curl_slist_append(*curl_headers, rawHeader.c_str());
}
