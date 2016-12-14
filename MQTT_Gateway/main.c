/*----- Header-Files ---------------------------------------------------------*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "ble_api.h"
#include "json.h"
#include "broker_api.h"

/*----- Defines --------------------------------------------------------------*/

/*----- Globals --------------------------------------------------------------*/
// connection and listener object
connection_t conn;
pthread_t listener;
pthread_mutex_t lock_recv;

/*----- Function prototypes --------------------------------------------------*/
void event_parser(json_t *jmsg);
static void *threaded_listener(void *pdata);

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/


/*******************************************************************************
 *  Callback :    recieved temperature data
 ******************************************************************************/
void on_temperature_data_recieved(json_t *msg)
{
    char str[20];
    message_t tempMessage;

    // Get subobject
    json_t *jdata = json_object_get(msg, "data");
    if(jdata == NULL){
        return;
    }

    // Get Value as json object
    json_t *jsonTemp = json_object_get(jdata, "value");
    if(jsonTemp == NULL){
        return;
    }

    // Get temperature and convert it to a string
    int pcValue = json_integer_value(jsonTemp);
    sprintf(str, "%d", pcValue);

    // Debug message
    debug(MSG_VAL, str);

    tempMessage.payload = str;
    tempMessage.topic = TOPIC_TEMP;

    sendMQTTmessage(&tempMessage);

    json_cleanup(jdata);
    json_cleanup(jsonTemp);
}

/*******************************************************************************
 *  Callback :    recieved acceleration data
 ******************************************************************************/
void on_accel_data_recieved(json_t *msg)
{
    char strX[20];
    char strY[20];
    char strZ[20];
    message_t tempMessage;

    // Get subobject
    json_t *jdata = json_object_get(msg, "data");
    if(jdata == NULL){
        return;
    }

    // Unpack xyz as json objects
    json_t *jsonTempX = json_object_get(jdata, "x");
    if(jsonTempX == NULL){
        return;
    }
    json_t *jsonTempY = json_object_get(jdata, "y");
    if(jsonTempY == NULL){
        return;
    }
    json_t *jsonTempZ = json_object_get(jdata, "z");
    if(jsonTempZ == NULL){
        return;
    }

    // Get numerical values and convert them back to strings
    int x = json_integer_value(jsonTempX);
    int y = json_integer_value(jsonTempY);
    int z = json_integer_value(jsonTempZ);

    // Copy to stringbuffer
    sprintf(strX, "%d", x);
    sprintf(strY, "%d", y);
    sprintf(strZ, "%d", z);

    // Debug Messages
    debug(MSG_VAL, strX);
    debug(MSG_VAL, strY);
    debug(MSG_VAL, strZ);

    tempMessage.payload = strX;
    tempMessage.topic = TOPIC_AX;
    sendMQTTmessage(&tempMessage);

    tempMessage.payload = strY;
    tempMessage.topic = TOPIC_AY;
    sendMQTTmessage(&tempMessage);

    tempMessage.payload = strZ;
    tempMessage.topic = TOPIC_AZ;
    sendMQTTmessage(&tempMessage);

    json_cleanup(jdata);
    json_cleanup(jsonTempX);
    json_cleanup(jsonTempY);
    json_cleanup(jsonTempZ);
}

/*******************************************************************************
 *  Callback :    recieved gyroscope data
 ******************************************************************************/
void on_gyro_data_recieved(json_t *msg)
{
    char strX[20];
    char strY[20];
    char strZ[20];
    message_t tempMessage;

    // Get subobject
    json_t *jdata = json_object_get(msg, "data");
    if(jdata == NULL){
        return;
    }

    // Unpack xyz as json objects
    json_t *jsonTempX = json_object_get(jdata, "x");
    if(jsonTempX == NULL){
        return;
    }
    json_t *jsonTempY = json_object_get(jdata, "y");
    if(jsonTempY == NULL){
        return;
    }
    json_t *jsonTempZ = json_object_get(jdata, "z");
    if(jsonTempZ == NULL){
        return;
    }

    // Get numerical values and convert them back to strings
    int x = json_integer_value(jsonTempX);
    int y = json_integer_value(jsonTempY);
    int z = json_integer_value(jsonTempZ);

    // Copy to stringbuffer
    sprintf(strX, "%d", x);
    sprintf(strY, "%d", y);
    sprintf(strZ, "%d", z);

    // Debug Messages
    debug(MSG_VAL, strX);
    debug(MSG_VAL, strY);
    debug(MSG_VAL, strZ);

    tempMessage.payload = strX;
    tempMessage.topic = TOPIC_GX;
    //sendMQTTmessage(&tempMessage);

    tempMessage.payload = strY;
    tempMessage.topic = TOPIC_GY;
    //sendMQTTmessage(&tempMessage);

    tempMessage.payload = strZ;
    tempMessage.topic = TOPIC_GZ;
    //sendMQTTmessage(&tempMessage);

    json_cleanup(jdata);
    json_cleanup(jsonTempX);
    json_cleanup(jsonTempY);
    json_cleanup(jsonTempZ);
}

/*******************************************************************************
 *  Callback :    sensor has been connected
 ******************************************************************************/
void on_sensor_connected(void)
{
    debug(MSG_EVNT, "Sensor connected.");

    if(!conn.is_connected){
        conn.is_connected = true;
    }
}

/*******************************************************************************
 *  Callback :    sensor has been disconnected
 ******************************************************************************/
void on_sensor_disconnected(void)
{
    debug(MSG_EVNT, "Sensor disconnected.");

    if(conn.is_connected){
        conn.is_connected = false;
    }
}

/*******************************************************************************
 *  function :    event_parser
 ******************************************************************************/
/** @brief        Searches for the event-key and then parses its value.
 *
 *  @type         global
 *
 *  @param[in]    jmsg     json event object
 *
 *  @return       none
 *
 ******************************************************************************/
void event_parser(json_t *jmsg)
{
    char *pcValue;

    pcValue = json_getStringValue(jmsg, "event");

    // Cancel if pcValue is a NULLpointer
    if(pcValue == NULL){
        return;
    }

    // Decide what to do
    if(strcmp(pcValue, "DeviceDiscovered") == 0){
        debug(MSG_EVNT, "New device discovered!");
    }
    if(!strcmp(pcValue, "DeviceConnected")){
        debug(MSG_EVNT, "Connected to a device!");
        on_sensor_connected();
    }
    if(!strcmp(pcValue, "DeviceDisconnected")){
        debug(MSG_EVNT, "Disconnected from a device!");
        on_sensor_disconnected();
    }
    if(!strcmp(pcValue, "MeasurementStopped")){
        debug(MSG_EVNT, "Measurement stopped!");
        conn.is_measuring = false;
    }
    if(!strcmp(pcValue, "TempConfigured")){
        debug(MSG_EVNT, "Temp sampler configured!");
        conn.temp_configured = true;
    }
    if(!strcmp(pcValue, "AccelConfigured")){
        debug(MSG_EVNT, "Accel sampler configured!");
        conn.accel_configured = true;
    }
    if(!strcmp(pcValue, "GyroConfigured")){
        debug(MSG_EVNT, "Gyro sampler configured!");
        conn.gyro_configured = true;
    }
    if(!strcmp(pcValue, "Temperature")){
        debug(MSG_EVNT, "Got temperature value");
        on_temperature_data_recieved(jmsg);
    }
    if(!strcmp(pcValue, "AccelData")){
        debug(MSG_EVNT, "Got accel value");
        on_accel_data_recieved(jmsg);
    }
    if(!strcmp(pcValue, "GyroData")){
        debug(MSG_EVNT, "Got gyro value");
        on_gyro_data_recieved(jmsg);
    }
}

/*******************************************************************************
 *  function :    threaded_listener
 ******************************************************************************/
/** @brief        Runs as seperate thread, listens on the socket and reacts on
 *                incoming messages.
 *
 *  @type         global
 *
 *  @param[in]    pdata     argument for the thread, not yet sure if used
 *
 *  @return       none
 *
 ******************************************************************************/
static void *threaded_listener(void *pdata)
{
    char tmp[STRING_SIZE];
    int i=0;

    while(true){
        // Try to read a response from the socket and store it in tmp
        pthread_mutex_lock(&lock_recv);
        int strlength = recv(conn.socket_fd, tmp, STRING_SIZE-1, 0);
        pthread_mutex_unlock(&lock_recv);

        int laststart = 0;
        for(i = 0; i < strlength; i++) {
            if(tmp[i]=='\n') { //end of one message
                tmp[i] = '\0'; //terminate message here
                debug(MSG_RECV, &tmp[laststart]);

                // parse json msg
                json_t *jmsg = json_createFromString(&tmp[laststart]);
                if(jmsg != NULL){
                    // call event parser
                    event_parser(jmsg);
                    // cleanup
                    json_cleanup(jmsg);
                }

                laststart=i+1;
            }
        }
        usleep(1000L);
    }
    pthread_exit(NULL);
}

/*******************************************************************************
 *  function :    main
 ******************************************************************************/
/** @brief        Entrypoint for the program
 *
 *  @type         global
 *
 *  @param[in]    argc     argument counter
 *  @param[in]    argv     argument vector
 *
 *  @return       EXIT_SUCCESS
 *
 ******************************************************************************/
int main(int argc, char **argv)
{
    // Connecting to Broker
    startBroker();

    message_t tempMessage;
    tempMessage.payload = PAYLOAD;
    tempMessage.topic = TOPIC;
    sendMQTTmessage(&tempMessage);

    // Init the connection object and connect to sensor-hub socket
    init_connect_obj(&conn);
    socket_get_connection(&conn);

    // Init mutexes
    pthread_mutex_init(&lock_recv, NULL);
    pthread_mutex_init(&conn.lock_send, NULL);

    // Set up listener thread
    pthread_create(&listener, NULL, threaded_listener, &conn);

    //Force disconnect
    //sensor_force_disconnect(&conn, SENSOR_MAC);

    // Do a ble scan
    sensor_get_ble_scan(&conn);

    // try to connect
    while(!conn.is_connected){
        sensor_connect(&conn, SENSOR_MAC);
        usleep(1000000L);
    }

    // Read temperature and gyro data from the ble sensor for 20s
    sensor_start_temperature_sampler(&conn, SENSOR_MAC);
    //sensor_start_gyroscope_sampler(&conn, SENSOR_MAC);
    //sensor_start_acceleration_sampler(&conn, SENSOR_MAC);

    // try to disconnect
    while(conn.is_connected){
        sensor_disconnect(&conn, SENSOR_MAC);
        usleep(1000000L);
    }

    // Cleanup
    pthread_cancel(listener);
    pthread_mutex_destroy(&lock_recv);
    pthread_mutex_destroy(&conn.lock_send);
    free_connect_obj(&conn);

    // disconnect from broker
    disconectBroker();

    // Exit
    return(EXIT_SUCCESS);
}
