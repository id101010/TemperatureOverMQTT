TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread

target.path = /home/pi
INSTALLS += target

SOURCES += main.c
