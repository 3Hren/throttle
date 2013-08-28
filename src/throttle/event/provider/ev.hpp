#pragma once

#include <ev++.h>

#include "utils.hpp"
#include "event/timer.hpp"

namespace throttle { namespace event { namespace provider {

struct Ev {
    typedef typename ev::loop_ref Loop;
    typedef typename throttle::event::Timer<Ev> Timer;

    Loop &loop;

private:
    DECLARE_NONCOPYABLE(Ev)
};

} } }
