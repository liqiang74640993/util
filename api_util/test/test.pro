TEMPLATE = app

include(../qt-common-config.pri)

INCLUDEPATH += ../../gtest/include \
            +=  ../thread   \

LIBS += -lgtest \
        -lgmock  \
        -lthread \
        -lcommon

LIBS += -L../../gtest/lib \
        -L../../bin

SOURCES += \
        ThreadTest.cpp \
        main.cpp
