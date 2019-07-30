include(gtest_dependency.pri)
include(../common.pri)

TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG += thread

HEADERS += \
    TimeslotTest.h

SOURCES += \
        main.cpp
