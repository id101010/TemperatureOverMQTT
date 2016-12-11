#include <stdio.h>
#include "ble_api.h"
#include "json.h"

int main(void)
{
    // Define a connection
    connection_t conn;

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

    // Exit
    return(EXIT_SUCCESS);
}
