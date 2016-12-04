CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread

target.path = /home/pi
INSTALLS += target

SOURCES += main.c \
    ble_api.c

HEADERS += \
    ble_api.h
