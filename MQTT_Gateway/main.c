#include <stdio.h>
#include "ble_api.h"

int main(void)
{
    // Define and init connection struct
    connection_t conn;
    init_connect_obj(&conn);

    // Connect to sensor hub socket
    socket_get_connection(&conn);

    // Read Temperature from ble sensor
    sensor_get_temperature(&conn);

    // Close socket
    close(conn.socket_fd);

    // Exit
    return(EXIT_SUCCESS);
}
