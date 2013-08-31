#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ev++.h>
#include <boost/format.hpp>
#include <boost/asio.hpp>
#include <rapidjson/document.h>

#define UNUSED(T) \
    (void)T;

using namespace ::testing;

#include "event/provider/ev.hpp"
#include "network/url.hpp"
#include "network/http/request.hpp"
#include "network/http/reply.hpp"
#include "network/http/manager.hpp"

using namespace throttle::network;
using namespace throttle::network::http;
using namespace throttle::event::provider;

#define TEST_OFF(__case__, __name__) \
    TEST(__case__, DISABLED_##__name__)
