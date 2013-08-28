#pragma once

#include <vector>
#include <list>
#include <functional>
#include <mutex>

namespace throttle { namespace async {

template<typename R>
class Deferred {
    std::vector<std::function<void(const R &reply)>> callbacks;
    std::list<R> cache;
    std::mutex mutex;
public:
    template<typename C>
    void addCallback(const C &callback) {
        std::unique_lock<std::mutex> lock(mutex);
        if (!cache.empty()) {
            for (auto it = cache.begin(); it != cache.end(); ++it) {
                R reply = *it;
                callback(reply);
            }
            return;
        }
        callbacks.push_back(callback);
    }

    void trigger(R &&reply) {
        std::unique_lock<std::mutex> lock(mutex);
        if (callbacks.empty()) {
            cache.push_back(reply);
        }

        for (auto it = callbacks.begin(); it != callbacks.end(); ++it) {
            auto callback = *it;
            callback(reply);
        }
    }
};

} }
