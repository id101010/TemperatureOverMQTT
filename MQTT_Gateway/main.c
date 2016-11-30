#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>

#define SENSOR_MAC "CC:85:C8:56:E5:DD"

#define START_BLE_SCAN "{\"device\":\"\",\n\"command\":\"StartBleScan\"\n}"
#define CONNECT_SENSOR "{\"device\":\""SENSOR_MAC"\",\"command\":\"Connect\"}"
#define DISCONNECT_SENSOR "{\"device\":\""SENSOR_MAC"\",\"command\":\"Disconnect\"}"

// String buffer
char buffer[100];

void send_command(int socket_fd, char *command)
{
    strcpy(buffer, command);
    send(socket_fd, buffer, strlen(buffer), 0);
    printf("[DEBUG]: Send command: %s.\n", buffer);
}

int main(void)
{
    int t = 0;

    printf("[DEBUG]: Hello.\n");

    // Get a socket filedescriptor
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    printf("[DEBUG]: Socket created.\n");

    // Set socket adress
    struct sockaddr_un socket_addr;
    socket_addr.sun_family = AF_UNIX;
    strcpy(socket_addr.sun_path, "/tmp/sensor-hub.socket");
    int addr_len = strlen(socket_addr.sun_path) + sizeof(socket_addr.sun_family);
    printf("[DEBUG]: Socket adress set.\n");

    // Connect to socket
    connect(socket_fd, (struct sockaddr *)&socket_addr, addr_len);
    printf("[DEBUG]: Socket connected.\n");


    // connect
    send_command(socket_fd, CONNECT_SENSOR);

    // Read response from socket
    t = recv(socket_fd, buffer, 99, 0);
    buffer[t] = '\0';
    printf("%s", buffer);

    sleep(10);

    // disconnect
    send_command(socket_fd, DISCONNECT_SENSOR);

    // Read response from socket
    t = recv(socket_fd, buffer, 99, 0);
    buffer[t] = '\0';
    printf("%s", buffer);

    // Close socket
    close(socket_fd);

    // Return
    return(EXIT_SUCCESS);
}
