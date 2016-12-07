#include <stdio.h>
#include "ble_api.h"
#include "json.h"

int main(void)
{
    // Test JSON stuff
    char *pcString = "{\" Hello \":\" World \"}";
    json_t *jsonMsg = NULL;
    jsonMsg = json_createFromString(pcString);

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
