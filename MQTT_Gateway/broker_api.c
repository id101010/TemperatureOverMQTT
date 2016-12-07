#include "broker_api.h"

void connlost(void *context, char *cause)
{
    printf("\nConnection lost cause:\n");
    printf("%s\n", cause);

    printf("Reconnecting\n");

    if((MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code \n");
        messageMQTT.finished = true;
    }
}

void onDisconnect(void* context, MQTTAsync_successData* response)
{
        printf("Successful disconnection\n");
        messageMQTT.finished = true;
}

void onSend(void* context, MQTTAsync_successData* response)
{
        printf("Message with token value %d delivery confirmed\n", response->token);
}

void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
        printf("Connect failed, rc %d\n", response ? response->code : 0);
        messageMQTT.finished = true;
}

void onConnect(void* context, MQTTAsync_successData* response)
{
        printf("Successful connection\n");
        messageMQTT.isConected = true;
}

void setConnectOptions()
{
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    conn_opts.password = PASSWORD;
    conn_opts.username = USERNAME;
}

void disconectBroker()
{

    MQTTAsync_destroy(&client);
}

void conectToBroker()
{
    MQTTAsync_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTAsync_setCallbacks(client, NULL, connlost, NULL, NULL);

    setConnectOptions();

    if ((MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
            printf("Failed to start connect\n");
            exit(-1);
    }
}

void sendMQTTmessage(message_t *message)
{
    pubmsg.payload = message->payload;
    pubmsg.payloadlen = strlen(message->payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;

    if(message->isConected != false)
    {
       if(MQTTAsync_sendMessage(client, message->topic, &pubmsg, &opts) != MQTTASYNC_SUCCESS)
       {
           printf("Failed to start sendMQTTmessage");
       }
    }
    else
    {
        printf("Can't send message, no connection established");
    }
}
