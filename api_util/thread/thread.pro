TEMPLATE = lib

include(../qt-common-config.pri)

SOURCES += \
    ThreadPool.cpp \
    ThreadPoolPosix.c \
    mythread.cpp

HEADERS += \
    ThreadPool.h \
    ThreadPoolPosix.h \
    mythread.h
