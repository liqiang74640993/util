TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../gtest/include

LIBS += -lgtest \
        -lgmock

LIBS += -L../../gtest/lib

SOURCES += \
        main.cpp
