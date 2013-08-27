CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_BUILD_TYPE "Debug")

SET(PROJECT hurl-tests)

SET(HEADERS
    Global.hpp
    Mocks.hpp
)

SET(SOURCES
    main.cpp
#    test_NetworkRequest.cpp
#    test_AsyncRequest.cpp
    test_Manual.cpp
)

SET(BOOST_COMPONENTS
    system
)

SET(GOOGLE_LIBRARIES
    gtest
    gmock
)

PROJECT(${PROJECT})

INCLUDE_DIRECTORIES(../throttle)

ADD_EXECUTABLE(${PROJECT} ${HEADERS} ${SOURCES})

TARGET_LINK_LIBRARIES(${PROJECT}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${GOOGLE_LIBRARIES}
    throttle
    ev
)

ADD_DEFINITIONS(-std=c++11)
