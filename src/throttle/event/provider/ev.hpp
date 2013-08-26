#pragma once

#include <ev++.h>

#include "event/timer.hpp"

namespace throttle { namespace event { namespace provider {

struct Ev {
    typedef typename ev::loop_ref Loop;
    typedef typename throttle::event::Timer<Ev> Timer;

    Loop &loop;
};

} } }
