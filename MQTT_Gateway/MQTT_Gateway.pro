CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread
LIBS += -lpthread
LIBS += -L$$PWD/lib/ -lpaho-mqtt3a

target.path = /home/pi
INSTALLS += target

SOURCES += main.c \
    ble_api.c \
    broker_api.c

HEADERS += \
    ble_api.h \
    broker_api.h \
    libs/MQTTAsync.h
