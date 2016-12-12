#ifndef BLE_API_H
#define BLE_API_H

/*
 *  function    debug
 *              init_connect_obj
 *              socket_get_connection
 *              send_command
 *              recieve_answer
 *              sensor_connect
 *              sensor_disconnect
 *              sensor_scan
 *              sensor_get_temperature
 *
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <json.h>

/*----- Macros ---------------------------------------------------------------*/
#define DEBUG

#define STRING_SIZE         1000
#define SOCKET_PATH         "/tmp/sensor-hub.socket"
#define SENSOR_MAC          "CC:85:C8:56:E5:DD"

#define MSG_RECV            1
#define MSG_DBG             2
#define MSG_SENT            3
#define MSG_EVNT            4

/*----- Data types -----------------------------------------------------------*/
typedef struct connection_s {
    // Control section
    bool is_connected;
    pthread_mutex_t *lock_send;

    // Command section
    char command[STRING_SIZE];
    char answer[STRING_SIZE];
    json_t *jsonMsg;

    // Socket section
    int socket_fd;
    struct sockaddr_un socket_addr;
    int addr_len;

} connection_t;

/*----- Function prototypes --------------------------------------------------*/
// Misc helper functions
void debug(int type, const char *msg);
void init_connect_obj(connection_t *conn);
void free_connect_obj(connection_t *conn);
void socket_get_connection(connection_t *conn);

// Atomic functions
void send_command(connection_t *conn, json_t *jsonMsg);
bool recieve_answer(connection_t *conn, char *output); /* DEBUG ONLY! */

// Sensor Control functions
void sensor_connect(connection_t *conn, char *sensor_mac);
void sensor_disconnect(connection_t *conn, char *sensor_mac);
void sensor_force_disconnect(connection_t *conn, char *sensor_mac);

// Single measurements
void sensor_get_ble_scan(connection_t *conn);
void sensor_get_single_temperature(connection_t *conn, char *sensor_mac);

// Data samplers
void sensor_start_temperature_sampler(connection_t *conn, char *sensor_mac);
void sensor_start_acceleration_sampler(connection_t *conn, char *sensor_mac);
void sensor_start_gyroscope_sampler(connection_t *conn, char *sensor_mac);

#endif // BLE_API_H
