#pragma once

#include <ev++.h>

#include "utils.hpp"

namespace throttle { namespace event {

namespace provider {
struct Ev;
}

template<typename T> struct Timer;

template<>
class Timer<event::provider::Ev> {
    DECLARE_NONCOPYABLE(Timer)

    ev::timer timer;
    const std::function<void(int)> callback;

public:
    Timer(ev::loop_ref &loop, const std::function<void(int)>& callback) :
        timer(loop),
        callback(callback)
    {
        timer.set<Timer, &Timer::onEvent>(this);
    }

    inline void onEvent(ev::timer &, int ec) {
        callback(ec);
    }

    inline void stop() {
        timer.stop();
    }

    inline void restart(int timeout) {
        stop();
        timer.set(timeout / 1000.0);
        timer.start();
    }
};

} }
