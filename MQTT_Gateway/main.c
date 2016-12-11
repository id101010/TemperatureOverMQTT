/*----- Header-Files ---------------------------------------------------------*/
#include <stdio.h>
#include <pthread.h>
#include "ble_api.h"
#include "json.h"
#include "broker_api.h"

/*----- Defines --------------------------------------------------------------*/
#define RECV_TIMEOUT_SEC 5

/*----- Globals --------------------------------------------------------------*/
// connection and listener object
connection_t conn;
pthread_t listener;

/*----- Function prototypes --------------------------------------------------*/
void event_parser(json_t *jmsg);
static void *threaded_listener(void *pdata);

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

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
        int strlength = recv(conn.socket_fd, tmp, STRING_SIZE-1, 0);

        int laststart = 0;
        for(i = 0; i < strlength; i++) {
            if(tmp[i]=='\n') { //end of one message
                tmp[i] = '\0'; //terminate message here
                debug(RECV_MSG, &tmp[laststart]);
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
    //sensor_force_disconnect(&conn, SENSOR_MAC);
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
