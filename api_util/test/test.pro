TEMPLATE = app

include(../qt-common-config.pri)

INCLUDEPATH += ../../gtest/include \
            +=  ../thread   \
            += ../designPattern \
            += ../memory      \

LIBS += -lgtest \
        -lgmock  \
        -lthread \
        -lcommon \
        -ldesignPattern \
        -lmemory     \

LIBS += -L../../gtest/lib \
        -L../../bin

SOURCES += \
        DesignPattern.cpp \
        ProcessIPC.cpp \
        ThreadTest.cpp \
        main.cpp
