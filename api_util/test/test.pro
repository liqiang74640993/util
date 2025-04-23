TEMPLATE = app

include(../qt-common-config.pri)

INCLUDEPATH += ../../gtest/include \
            +=  ../thread   \
            += ../designPattern \

LIBS += -lgtest \
        -lgmock  \
        -lthread \
        -lcommon \
        -ldesignPattern

LIBS += -L../../gtest/lib \
        -L../../bin

SOURCES += \
        DesignPattern.cpp \
        ThreadTest.cpp \
        main.cpp
