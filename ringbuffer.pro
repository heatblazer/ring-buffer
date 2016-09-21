TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    concurent.cpp

HEADERS += \
    concurent.h \
    ringbuffer.h

LIBS += -lpthread
