TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += dead_pix2.cpp

INCLUDEPATH += ../common
LIBS += -L../common -lcommon

target.path = /usr/local/bin/
INSTALLS += target
