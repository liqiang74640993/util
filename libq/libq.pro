TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        mrp_common.c

include(../third/googletest-main/gtest_dependency.pri)

HEADERS += \
    mrp_common.h \
    mvrp_procuder.h
