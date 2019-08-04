include(gtest_dependency.pri)
include(../common.pri)

TEMPLATE = app
CONFIG += cmdline

HEADERS += \
    miscTest.h \
    TimeslotTest.h

SOURCES += \
        main.cpp
