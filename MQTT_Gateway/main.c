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
    debug(MSG_DBG, "Temperaure Data recieved.");

    /*
     * TODO: Send Data to Broker
     */
}

/*******************************************************************************
 *  Callback :    recieved acceleration data
 ******************************************************************************/
void on_accel_data_recieved(void)
{
    debug(MSG_DBG, "Acceleration Data recieved.");
    /*
     * TODO: Send Data to Broker
     */
}

/*******************************************************************************
 *  Callback :    recieved gyroscope data
 ******************************************************************************/
void on_gyro_data_recieved(void)
{
    debug(MSG_DBG, "Gyroscope Data recieved.");
    /*
     * TODO: Send Data to Broker
     */
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
    if(pcValue != NULL){
        return;
    }

    // Decide what to do
    if(strcmp(pcValue, "DeviceConnected")){
        on_sensor_connected();
    }
    if(strcmp(pcValue, "DeviceDisconnected")){
        on_sensor_disconnected();
    }
    if(strcmp(pcValue, "Temperature")){
        on_temperature_data_recieved();
    }
    if(strcmp(pcValue, "AccelData")){
        on_accel_data_recieved();
    }
    if(strcmp(pcValue, "GyroData")){
        on_gyro_data_recieved();
    }

    // Free ressources
    debug(MSG_EVNT, pcValue);
    json_freeString(pcValue);
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
        //pthread_lock_mutex(lock_recv);
        int strlength = recv(conn.socket_fd, tmp, STRING_SIZE-1, 0);
        //pthread_unlock_mutex(lock_recv);

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
    // Init the connection object and connect to sensor-hub socket
    init_connect_obj(&conn);
    socket_get_connection(&conn);

    // Set up listener thread
    pthread_create(&listener, NULL, threaded_listener, &conn);

    // Do a ble scan
    sensor_get_ble_scan(&conn);
    // Force disconnect
    sensor_force_disconnect(&conn, SENSOR_MAC);
    // Connect to sensor
    //sensor_connect(&conn, SENSOR_MAC);
    // Read Temperature from ble sensor
    //sensor_start_temperature_sampler(&conn, SENSOR_MAC);
    // Disconnect from sensor
    //sensor_disconnect(&conn, SENSOR_MAC);

    // Cleanup
    pthread_cancel(listener);
    free_connect_obj(&conn);
   
    // Exit
    return(EXIT_SUCCESS);
}
