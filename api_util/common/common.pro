TEMPLATE = lib
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += \
     define.h \
     ring_buffer.h

SOURCES += \
    ring_buffer.c

DESTDIR = ../../bin #generate
