CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_BUILD_TYPE "Debug")

SET(PROJECT throttle)

SET(HEADERS
    utils.hpp
    event/timer.hpp
    event/watcher.hpp
    event/provider/ev.hpp
    network/http/connection/info.hpp
    network/http/callbacks.hpp
    network/http/header.hpp
    network/http/request.hpp
    network/http/reply.hpp
    network/http/manager.hpp
)

SET(SOURCES
    network/http/connection/info.cpp
    network/http/request.cpp
    network/http/manager.cpp
)

SET(BOOST_COMPONENTS
    system
)

PROJECT($PROJECT)

FIND_PACKAGE(Boost COMPONENTS ${BOOST_COMPONENTS} REQUIRED)

INCLUDE_DIRECTORIES(.)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

ADD_LIBRARY(${PROJECT} SHARED
    ${HEADERS}
    ${SOURCES}
)

TARGET_LINK_LIBRARIES(${PROJECT}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    curl
    ev
)

SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES ENABLE_EXPORTS true)
ADD_DEFINITIONS(-std=c++11)

