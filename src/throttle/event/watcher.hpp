#pragma once

#include <ev++.h>

namespace throttle { namespace event {

namespace provider {
struct Ev;
}

template<typename T> struct Watcher;

template<>
class Watcher<event::provider::Ev> {
    ev::io watcher;
    const std::function<void(int, int)> callback;

public:
    Watcher(ev::loop_ref &loop, const std::function<void(int, int)>& callback) :
        watcher(loop),
        callback(callback)
    {
        watcher.set<Watcher, &Watcher::onEvent>(this);
    }

    inline void onEvent(ev::io &watcher, int events) {
        callback(watcher.fd, events);
    }

    inline int fd() const {
        return watcher.fd;
    }

    inline void process_events(int sockfd, int events) {
        const bool active = watcher.is_active();
        watcher.set(sockfd, events);
        if (!active)
            watcher.start();
    }
};

} }
