#include "ble_api.h"
#include "json.h"

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
    // Send Command
    strcpy(conn->command, command);
    send(conn->socket_fd, conn->command, STRING_SIZE, 0);
    debug(conn->command);
}

void recieve_answer(connection_t *conn)
{
    int length = 0;

    // Read response from socket
    length = recv(conn->socket_fd, conn->answer, STRING_SIZE-1, 0);
    conn->answer[length] = '\0'; // Append nullterminator
    printf(conn->answer);
}

void sensor_connect(connection_t *conn){
    // Connect to sensor if not already connected
    if(!conn->is_connected){
        send_command(conn, CMD_CONNECT);
        recieve_answer(conn);
        conn->is_connected = true;
    }
}

void sensor_disconnect(connection_t *conn){
    // Disconnect sensor if already connected
    if(conn->is_connected){
        send_command(conn, CMD_DISCONNECT);
        recieve_answer(conn);
        conn->is_connected = false;
    }
}

void sensor_scan(connection_t *conn)
{
    // Do a BLE Scan
    send_command(conn, CMD_START_BLE_SCAN);
    recieve_answer(conn);
}

void sensor_get_temperature(connection_t *conn){
    // connect
    sensor_connect(conn);
    // Enable gyroscope (this needs to be done for accurate temperature!)
    send_command(conn, CMD_ENABLE_GYRO);
    recieve_answer(conn);
    // request temperature
    send_command(conn, CMD_GETTEMPERATURE);
    recieve_answer(conn);
    // disconnect
    sensor_disconnect(conn);
}
