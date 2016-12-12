/*----- Header-Files ---------------------------------------------------------*/
#include <stdio.h>
#include <pthread.h>
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
void on_temperature_data_recieved(void)
{
    char dummy_temp[10] = "";
    message_t tempMessage;

    debug(MSG_DBG, "Temperaure Data recieved.");

    tempMessage.payload = dummy_temp;
    tempMessage.topic = TOPIC_TEMP;

    sendMQTTmessage(&tempMessage);
}

/*******************************************************************************
 *  Callback :    recieved acceleration data
 ******************************************************************************/
void on_accel_data_recieved(void)
{
    char dummy_ax[10] = "";
    char dummy_ay[10] = "";
    char dummy_az[10] = "";
    message_t tempMessage;

    debug(MSG_DBG, "Acceleration Data recieved.");

    tempMessage.payload = dummy_ax;
    tempMessage.topic = TOPIC_AX;
    sendMQTTmessage(&tempMessage);

    tempMessage.payload = dummy_ay;
    tempMessage.topic = TOPIC_AY;
    sendMQTTmessage(&tempMessage);

    tempMessage.payload = dummy_az;
    tempMessage.topic = TOPIC_AZ;
    sendMQTTmessage(&tempMessage);
}

/*******************************************************************************
 *  Callback :    recieved gyroscope data
 ******************************************************************************/
void on_gyro_data_recieved(void)
{
    char dummy_gx[10] = "";
    char dummy_gy[10] = "";
    char dummy_gz[10] = "";
    message_t tempMessage;

    debug(MSG_DBG, "Gyroscope Data recieved.");

    tempMessage.payload = dummy_gx;
    tempMessage.topic = TOPIC_GX;
    sendMQTTmessage(&tempMessage);

    tempMessage.payload = dummy_gy;
    tempMessage.topic = TOPIC_GY;
    sendMQTTmessage(&tempMessage);

    tempMessage.payload = dummy_gz;
    tempMessage.topic = TOPIC_GZ;
    sendMQTTmessage(&tempMessage);
}

/*******************************************************************************
 *  Callback :    sensor has been connected
 ******************************************************************************/
void on_sensor_connected(void)
{
    debug(MSG_DBG, "Sensor connected.");

    if(!conn.is_connected){
        conn.is_connected = true;
    }
}

/*******************************************************************************
 *  Callback :    sensor has been disconnected
 ******************************************************************************/
void on_sensor_disconnected(void)
{
    debug(MSG_DBG, "Sensor disconnected.");

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
    if(!strcmp(pcValue, "MeasurementStopped")){
        debug(MSG_EVNT, "Measurement stopped!");
        conn.is_measuring = false;
        on_sensor_connected();
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
    if(!strcmp(pcValue, "DeviceDisconnected")){
        on_sensor_disconnected();
        debug(MSG_EVNT, "Disconnected from a device!");
    }
    if(!strcmp(pcValue, "Temperature")){
        //on_temperature_data_recieved();
        debug(MSG_EVNT, "Got temperature value");
    }
    if(!strcmp(pcValue, "AccelData")){
        //on_accel_data_recieved();
        debug(MSG_EVNT, "Got accel value");
    }
    if(!strcmp(pcValue, "GyroData")){
        //on_gyro_data_recieved();
        debug(MSG_EVNT, "Got gyro value");
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
    //startBroker();

    // Init the connection object and connect to sensor-hub socket
    init_connect_obj(&conn);
    socket_get_connection(&conn);

    // Init mutexes
    pthread_mutex_init(&lock_recv, NULL);
    pthread_mutex_init(&conn.lock_send, NULL);

    // Set up listener thread
    pthread_create(&listener, NULL, threaded_listener, &conn);

    // Do a ble scan
    sensor_get_ble_scan(&conn);
    sleep(5);

    // Force disconnect
    sensor_force_disconnect(&conn, SENSOR_MAC);

    // try to connect
    while(!conn.is_connected){
        sensor_connect(&conn, SENSOR_MAC);
        usleep(500000L);
    }

    // Read temperature and gyro data from the ble sensor for 20s
    sensor_start_temperature_sampler(&conn, SENSOR_MAC);

    // try to disconnect
    while(!conn.is_connected){
        sensor_disconnect(&conn, SENSOR_MAC);
        usleep(500000L);
    }

    // wait for a disconnect
    while(conn.is_connected);

    // Cleanup
    pthread_cancel(listener);
    pthread_mutex_destroy(&lock_recv);
    pthread_mutex_destroy(&conn.lock_send);
    free_connect_obj(&conn);
    //disconectBroker();
    // Exit
    return(EXIT_SUCCESS);
}
