#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define UNUSED(T) \
    (void)T;

using namespace ::testing;

#include "network_request.hpp"
#include "HttpRequestManager.hpp"
