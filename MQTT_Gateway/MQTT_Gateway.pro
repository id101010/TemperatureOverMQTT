CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread
LIBS += -L$$PWD/libs/libpaho -lpaho-mqtt3a
LIBS += -L$$PWD/libs/jansson/lib -ljansson

target.path = /home/pi
INSTALLS += target

SOURCES += main.c \
    ble_api.c \
    broker_api.c

HEADERS += \
    ble_api.h \
    broker_api.h \
    libs/jansson/include/jansson_config.h \
    libs/jansson/include/jansson.h \
    libs/libpaho/MQTTAsync.h \
    libs/libpaho/MQTTClient.h \
    libs/libpaho/MQTTClientPersistence.h

DISTFILES +=
