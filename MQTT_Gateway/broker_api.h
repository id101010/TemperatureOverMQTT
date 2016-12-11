#ifndef BROKER_API_H
#define BROKER_API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libs/libpaho/MQTTAsync.h>

#define ADDRESS     "tcp://iot.i3s.bfh.ch:1883"
#define CLIENTID    "BLESensor"
#define TOPIC       "EmbSy/gruppe_15/test"
#define TOPIC_TEMP  "EmbSy/gruppe_15/temp"
#define TOPIC_AX    "EmbSy/gruppe_15/ax"
#define TOPIC_AY    "EmbSy/gruppe_15/ay"
#define TOPIC_AZ    "EmbSy/gruppe_15/az"
#define USERNAME    "gruppe_15"
#define PASSWORD    "JfC65L05"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     100L

typedef struct message_s{
    char *payload;
    char *topic;

}message_t;


void startBroker();
void sendMQTTmessage(message_t *message);
void waitForDelivering();
void disconectBroker();


#endif // BROKER_API_H
