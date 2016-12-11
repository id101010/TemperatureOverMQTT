#include <stdio.h>
#include "ble_api.h"
#include "json.h"
#include "broker_api.h"

int main(void)
{
    // Define a connection
    connection_t conn;


    // Broker
    message_t message;

    // Init the connection object
    init_connect_obj(&conn);

    // Connect to sensor hub socket
    socket_get_connection(&conn);

    // Do a ble scan
    sensor_ble_scan(&conn);

    // Connect to sensor
    sensor_connect(&conn, SENSOR_MAC);

    // Read Temperature from ble sensor
    sensor_get_temperature(&conn, SENSOR_MAC);

    // Disconnect from sensor
    sensor_disconnect(&conn, SENSOR_MAC);

    // Close socket
    close(conn.socket_fd);

    // Broker
    message.payload = "Hello World!!!!";
    message.topic = TOPIC;

    startBroker();
    sendMQTTmessage(&message);
    disconectBroker();
    // Exit
    return(EXIT_SUCCESS);
}
