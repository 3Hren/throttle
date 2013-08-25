#pragma once

#include <ev++.h>

struct EvProvider;

template<typename Provider> struct Timer;

template<>
struct Timer<EvProvider> {
    ev::timer timer;
    const std::function<void(int)> callback;

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
