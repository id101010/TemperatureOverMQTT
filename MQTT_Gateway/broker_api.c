#include "broker_api.h"

int finished_MQTT = 0;
int connected = 0;
int rc;
volatile MQTTAsync_token deliveredtoken;
MQTTAsync client;
MQTTAsync_token token;
MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
MQTTAsync_disconnectOptions disconn_opts = MQTTAsync_disconnectOptions_initializer;
MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
MQTTAsync_responseOptions response_opts = MQTTAsync_responseOptions_initializer;

void connlost(void *context, char *cause)
{

    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
    fflush(stdout);

    printf("Reconnecting\n");
    fflush(stdout);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        fflush(stdout);
        finished_MQTT = 1;
    }
}


void onDisconnect(void* context, MQTTAsync_successData* response)
{
    printf("Successful disconnection\n");
    fflush(stdout);
    finished_MQTT = 1;
    connected = 0;

}


void onSend(void* context, MQTTAsync_successData* response)
{
    printf("Message with token value %d delivery confirmed\n", response->token);
    fflush(stdout);
    finished_MQTT = 1;
}


void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    printf("Connect failed, rc %d\n", response ? response->code : 0);
    fflush(stdout);
    finished_MQTT = 1;
    connected = 0;
}

void onConnect(void* context, MQTTAsync_successData* response)
{
    printf("Successful connection\n");
    fflush(stdout);
    connected = 1;
}

void startBroker()
{
    MQTTAsync_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTAsync_setCallbacks(client, NULL, connlost, NULL, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;

    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
          printf("Failed to start connect, return code %d\n", rc);
          fflush(stdout);
          exit(-1);
    }

    printf("Waiting for connection to MQTT Broker\n");
    fflush(stdout);
    while(!connected)
    {
        #if defined(WIN32) || defined(WIN64)
            Sleep(100);
        #else
        usleep(TIMEOUT);
        #endif
    }
}

void sendMQTTmessage(message_t *message)
{
    response_opts.onSuccess = onSend;
    response_opts.context = client;

    pubmsg.payload = message->payload;
    pubmsg.payloadlen = strlen(message->payload);
    pubmsg.qos =  QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;

    if ((rc = MQTTAsync_sendMessage(client, message->payload, &pubmsg, &response_opts)) != MQTTASYNC_SUCCESS)
    {        
        printf("Failed to start sendMessage, return code %d\n", rc);
        fflush(stdout);
        exit(-1);
    }

    printf("Waiting for publication of %s\n"
    "on topic %s for client with ClientID: %s\n",
    message->payload, message->topic, CLIENTID);
    fflush(stdout);
    while (!finished_MQTT)
    {
        usleep(TIMEOUT);
    }
}

void disconectBroker()
{
    disconn_opts.onSuccess = onDisconnect;
    disconn_opts.context = client;

    if ((rc = MQTTAsync_disconnect(client, &disconn_opts)) != MQTTASYNC_SUCCESS)
    {
            printf("Failed to disconnect from Broker, return code %d\n", rc);
            fflush(stdout);
            exit(-1);
    }
    printf("Waiting for disconnection from MQTT Broker\n");
    while(connected)
    {
        usleep(TIMEOUT);
    }

    MQTTAsync_destroy(&client);
}
