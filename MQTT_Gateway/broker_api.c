/*----- Header-Files ---------------------------------------------------------*/
#include "broker_api.h"

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/
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

/*----- Implementation -------------------------------------------------------*/

/*******************************************************************************
 *  function :    connlost
 ******************************************************************************/
/** @brief        function is executed when a connection is lost and trys to
 *                reconnect
 *
 *  @type         private
 *
 *  @param[in]    contex: contains any application-specific context
 *                cause: reason for disconnecting
 *
 *  @return       none
 *
 *  @author       Marcus Schluep
 *
 ******************************************************************************/
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

/*******************************************************************************
 *  function :    onDisconnect
 ******************************************************************************/
/** @brief        function is executed when Broker is succesfull disconnected
 *
 *  @type         private
 *
 *  @param[in]    contex: contains any application-specific context
 *                response: respons from MQTTAsync_disconnect();
 *
 *  @return       none
 *
 *  @author       Marcus Schluep
 *
 ******************************************************************************/
void onDisconnect(void* context, MQTTAsync_successData* response)
{
    printf("Successful disconnection\n");
    fflush(stdout);
    finished_MQTT = 1;
    connected = 0;

}

/*******************************************************************************
 *  function :    onSend
 ******************************************************************************/
/** @brief        function is executed when a message is send succesfull
 *
 *
 *  @type         private
 *
 *  @param[in]    contex: contains any application-specific context
 *                response: respons from MQTTAsync_sendMessage();
 *
 *  @return       none
 *
 *  @author       Marcus Schluep
 *
 ******************************************************************************/
void onSend(void* context, MQTTAsync_successData* response)
{
    //debug(MSG_SENT, "Message delivery confirmed");
    //debug(MSG_DBG, response->alt.pub.message.payload);
    finished_MQTT = 1;
}

/*******************************************************************************
 *  function :    onConnectFailure
 ******************************************************************************/
/** @brief        function is executed when a sccesfull connection is
 *                established
 *
 *  @type         private
 *
 *  @param[in]    contex: A pointer to the context value originally passed
 *                          to MQTTAsync_responseOptions, which contains any
 *                          application-specific context
 *                response: respons from MQTTAsync_connect();
 *
 *  @return       none
 *
 *  @author       Marcus Schluep
 *
 ******************************************************************************/
void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    printf("Connect failed, rc %d\n", response ? response->code : 0);
    fflush(stdout);
    finished_MQTT = 1;
    connected = 0;
}

/*******************************************************************************
 *  function :    onConnect
 ******************************************************************************/
/** @brief        function is executed when a successfull connection is
 *                established
 *
 *  @type         private
 *
 *  @param[in]    contex: A pointer to the context value originally passed
 *                          to MQTTAsync_responseOptions, which contains any
 *                          application-specific context
 *                response: respons from MQTTAsync_connect();
 *
 *  @return       none
 *
 *  @author       Marcus Schluep
 *
 ******************************************************************************/
void onConnect(void* context, MQTTAsync_successData* response)
{
    printf("Successful connection\n");
    fflush(stdout);
    connected = 1;
}

/*******************************************************************************
 *  function :    startBroker
 ******************************************************************************/
/** @brief         connect to broker
 *
 *
 *  @type          global
 *
 *  @param[in]     none
 *
 *  @return        none
 *
 *  @author       Marcus Schluep
 *
 ******************************************************************************/
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
/*******************************************************************************
 *  function :    sendMQTTmesage
 ******************************************************************************/
/** @brief        send a message to broker
 *
 *
 *  @type         global
 *
 *  @param[in]    struct message_t
 *
 *  @return       none
 *
 *  @author       Marcus Schluep
 *
 ******************************************************************************/
void sendMQTTmessage(message_t *message)
{
    response_opts.onSuccess = onSend;
    response_opts.context = client;

    pubmsg.payload = message->payload;
    pubmsg.payloadlen = strlen(message->payload);
    pubmsg.qos =  QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;

    if ((rc = MQTTAsync_sendMessage(client, message->topic, &pubmsg, &response_opts)) != MQTTASYNC_SUCCESS)
    {        
        debug(MSG_EVNT, "Failed to start send message");
        exit(-1);
    }
    debug(MSG_EVNT, "Waiting for publication");
    while (!finished_MQTT)
    {
        usleep(TIMEOUT);
    }
}
/*******************************************************************************
 *  function :    disconectBroker
 ******************************************************************************/
/** @brief        disconnects the broker and kills the client
 *
 *  @type         global
 *
 *  @param[in]    none
 *
 *  @return       none
 *
 *  @author       Marcus Schluep
 *
 ******************************************************************************/
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
