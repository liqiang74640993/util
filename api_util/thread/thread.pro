TEMPLATE = lib

include(../qt-common-config.pri)

SOURCES += \
    HThread.cpp \
    ProducerConsumerPosix.c \
    ThreadPool.cpp \
    ThreadPoolPosix.c

HEADERS += \
    HThread.h \
    ProducerConsumerPosix.h \
    ThreadPool.h \
    ThreadPoolPosix.h \
