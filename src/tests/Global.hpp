#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define UNUSED(T) \
    (void)T;

using namespace ::testing;

#include "event/provider/ev.hpp"
#include "network/http/request.hpp"
#include "network/http/manager.hpp"

using namespace throttle::network::http;
using namespace throttle::event::provider;
