TEMPLATE = lib
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += \
     c_ring_buffer.h \
     cplus_ring_buffer.h \
     define.h

SOURCES += \
    c_ring_buffer.cpp

DESTDIR = ../../bin #generate
