TARGET = raw2nef
SOURCES = write_nef2.cpp
INCLUDEPATH += ../common
LIBS += -L../common -lcommon
CONFIG += console c++11

target.path = /usr/local/bin/
INSTALLS += target
