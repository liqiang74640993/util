CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD/./common/

DESTDIR = ../../bin #generate

LIBS += -lcommon
LIBS += -L../../bin  \
