#ifndef BROKER_API_H
#define BROKER_API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libs/MQTTAsync.h>

#define ADDRESS     "tcp://iot.i3s.bfh.ch:1883"
#define CLIENTID    "ExampleClient"
#define TOPICTemp   "EmbSy/gruppe_15/temp"
#define TOPICAX     "EmbSy/gruppe_15/ax"
#define TOPICAY     "EmbSy/gruppe_15/ay"
#define TOPICAZ     "EmbSy/gruppe_15/az"
#define USERNAME    "gruppe_15"
#define PASSWORD    "JfC65L05"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L




#endif // BROKER_API_H