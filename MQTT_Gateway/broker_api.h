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
#define TOPIC_TEMP   "EmbSy/gruppe_15/temp"
#define TOPIC_AX     "EmbSy/gruppe_15/ax"
#define TOPIC_AY     "EmbSy/gruppe_15/ay"
#define TOPIC_AZ     "EmbSy/gruppe_15/az"
#define USERNAME    "gruppe_15"
#define PASSWORD    "JfC65L05"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

#define STRING_SIZE 1000

MQTTAsync client;
MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
MQTTAsync_token token;

typedef struct message_s{
    // Control Section
    bool isConected;
    bool finished;

    char payload[STRING_SIZE];
    char topic[STRING_SIZE];


}message_t;

message_t messageMQTT;

volatile MQTTAsync_token deliveredtoken;

void sendMQTTmessage(message_t *message);
void conectToBroker();
void disconectBroker();

#endif // BROKER_API_H
