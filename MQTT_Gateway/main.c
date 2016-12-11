#include <stdio.h>
#include <pthread.h>
#include "ble_api.h"
#include "json.h"
#include "broker_api.h"

static void *temperature_thread(void *pdata)
{
    pthread_exit(NULL);
}

static void *acceleration_thread(void *pdata)
{
    pthread_exit(NULL);
}

int main(void)
{
    connection_t conn;

    // Broker
    message_t message;


    // Init the connection object
    init_connect_obj(&conn);
    // Connect to sensor hub socket
    socket_get_connection(&conn);
    // Do a ble scan
    sensor_get_ble_scan(&conn);


    // Connect to sensor
    if(sensor_connect(&conn, SENSOR_MAC)){
        debug("Sensor connected, yaaay!");
    }

    // Read Temperature from ble sensor
    //sensor_start_temperature_sampler(&conn, SENSOR_MAC);

    // Disconnect from sensor
    if(sensor_disconnect(&conn, SENSOR_MAC)){
        debug("Sensor disconnected, yaaay!");
    }

    // Cleanup
    free_connect_obj(&conn);

    // Broker
    message.payload = "Hello World!!!!";
    message.topic = TOPIC;

    startBroker();
    sendMQTTmessage(&message);
    disconectBroker();
    // Exit
    return(EXIT_SUCCESS);
}
