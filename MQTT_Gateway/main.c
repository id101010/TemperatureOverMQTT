#include <stdio.h>
#include <pthread.h>
#include "ble_api.h"
#include "json.h"

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
    pthread_t th_temp, th_accel;
    connection_t conn;

    // Create Threads
    pthread_create(&th_temp, NULL, temperature_thread, NULL);
    pthread_create(&th_accel, NULL, acceleration_thread, NULL);

    // Init the connection object
    init_connect_obj(&conn);
    // Connect to sensor hub socket
    socket_get_connection(&conn);
    // Do a ble scan
    sensor_get_ble_scan(&conn);

    // Try to disconnect if already connected
    //conn.is_connected = true;
    //sensor_disconnect(&conn, SENSOR_MAC);
    //conn.is_connected = false;

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

    // Stop measure threads
    pthread_cancel(th_temp);
    pthread_join(th_temp, NULL);
    pthread_cancel(th_accel);
    pthread_join(th_accel, NULL);

    // Exit
    return(EXIT_SUCCESS);
}
