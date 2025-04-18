TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


include(../third/googletest-main/gtest_dependency.pri)

SOURCES += \
        main.cpp
