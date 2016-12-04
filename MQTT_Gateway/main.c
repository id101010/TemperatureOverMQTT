//raspi-adc6.bfh.ch
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#define DEBUG

#define STRING_SIZE         1000
#define SOCKET_PATH         "/tmp/sensor-hub.socket"
#define SENSOR_MAC          "CC:85:C8:56:E5:DD"

#define CMD_START_BLE_SCAN  "{\"device\":\"\",\"command\":\"StartBleScan\"}"
#define CMD_CONNECT         "{\"device\":\""SENSOR_MAC"\",\"command\":\"Connect\"}"
#define CMD_DISCONNECT      "{\"device\":\""SENSOR_MAC"\",\"command\":\"Disconnect\"}"
#define CMD_GETDEVINFO      "{\"device\":\""SENSOR_MAC"\",\"command\":\"GetDeviceInfo\"}"
#define CMD_GETTEMPERATURE  "{\"device\":\""SENSOR_MAC"\",\"command\":\"GetTemperature\"}"
#define CMD_ENABLE_GYRO     "{\"device\":\""SENSOR_MAC"\",\"command\":\"ConfigGyro\",\"data\":{\"on\":true,\"fullscale\":500,\"odr\":95}}"

typedef struct connection_s {
    // Control section
    bool is_connected;

    // Command section
    char command[STRING_SIZE];
    char answer[STRING_SIZE];

    // Socket section
    int socket_fd;
    struct sockaddr_un socket_addr;
    int addr_len;

} connection_t;

void debug(const char *msg)
{
#ifdef DEBUG
    printf("[DEBUG]: %s\n", msg);
#endif
}

void init_connect_obj(connection_t *conn)
{
    conn->is_connected=false;
}

void socket_get_connection(connection_t *conn)
{
    // Get a socket filedescriptor
    conn->socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    debug("Socket created.");

    // Set socket adress
    conn->socket_addr.sun_family = AF_UNIX;
    strcpy(conn->socket_addr.sun_path, SOCKET_PATH);
    conn->addr_len = strlen(conn->socket_addr.sun_path) + sizeof(conn->socket_addr.sun_family);
    debug("Socket adress set.");

    // Connect to socket
    connect(conn->socket_fd, (struct sockaddr *)&(conn->socket_addr), (conn->addr_len));
    debug("Socket connected.");
}

void send_command(connection_t *conn, char *command)
{
    int length = 0;

    // Send Command
    strcpy(conn->command, command);
    send(conn->socket_fd, conn->command, STRING_SIZE, 0);
    debug(conn->command);

    // Read response from socket
    length = recv(conn->socket_fd, conn->answer, STRING_SIZE-1, 0);
    conn->answer[length] = '\0'; // Append nullterminator
    printf(conn->answer);
}

void sensor_connect(connection_t *conn){
    // Connect to sensor if not already connected
    if(!conn->is_connected){
        send_command(conn, CMD_CONNECT);
        conn->is_connected = true;
    }
}

void sensor_disconnect(connection_t *conn){
    // Disconnect sensor if already connected
    if(conn->is_connected){
        send_command(conn, CMD_DISCONNECT);
        conn->is_connected = false;
    }
}

void sensor_scan(connection_t *conn)
{
    // Do a BLE Scan
    send_command(conn, CMD_START_BLE_SCAN);
}

void sensor_get_temperature(connection_t *conn){
    // connect
    sensor_connect(conn);
    // Enable gyroscope (this needs to be done for accurate temperature!)
    send_command(conn, CMD_ENABLE_GYRO);
    // request temperature
    send_command(conn, CMD_GETTEMPERATURE);
    // disconnect
    sensor_disconnect(conn);
}

int main(void)
{
    connection_t conn;
    init_connect_obj(&conn);

    // Connect to sensor hub socket
    socket_get_connection(&conn);

    // Read Temperature from ble sensor
    sensor_get_temperature(&conn);

    // Close socket
    close(conn.socket_fd);

    // Return
    return(EXIT_SUCCESS);
}
