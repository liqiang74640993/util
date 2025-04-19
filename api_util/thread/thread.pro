TEMPLATE = lib

include(../qt-common-config.pri)

SOURCES += \
    ProducerConsumerPosix.c \
    ThreadPool.cpp \
    ThreadPoolPosix.c \
    mythread.cpp

HEADERS += \
    ProducerConsumerPosix.h \
    ThreadPool.h \
    ThreadPoolPosix.h \
    mythread.h
