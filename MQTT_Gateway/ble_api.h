#ifndef BLE_API_H
#define BLE_API_H

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

void debug(const char *msg);
void init_connect_obj(connection_t *conn);
void socket_get_connection(connection_t *conn);
void send_command(connection_t *conn, char *command);
void recieve_answer(connection_t *conn);
void sensor_connect(connection_t *conn);
void sensor_disconnect(connection_t *conn);
void sensor_scan(connection_t *conn);
void sensor_get_temperature(connection_t *conn);

#endif // BLE_API_H
