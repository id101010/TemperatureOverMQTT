/*----- Header-Files ---------------------------------------------------------*/
#include <sys/time.h>
#include "ble_api.h"
#include "json.h"

/*----- Defines --------------------------------------------------------------*/
#define RECV_TIMEOUT_SEC 10

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

/*******************************************************************************
 *  function :    debug
 ******************************************************************************/
/** @brief        Handles the printing of debug messages. Messages only get printed
 *                if DEBUG is defined in the header file.
 *
 *  @type         global
 *
 *  @param[in]    msg     Text string
 *
 *  @return       none
 *
 ******************************************************************************/
void debug(const char *msg)
{
#ifdef DEBUG
    printf("[DEBUG]: %s\n", msg);
    fflush(stdout);
#endif
}

/*******************************************************************************
 *  function :    init_connect_obj
 ******************************************************************************/
/** @brief        Initializes a connection object.
 *
 *  @type         global
 *
 *  @param[in]    conn     Connection object
 *
 *  @return       none
 *
 ******************************************************************************/
void init_connect_obj(connection_t *conn)
{
    conn->is_connected=false;
}

/*******************************************************************************
 *  function :    socket_get_connection
 ******************************************************************************/
/** @brief        Creates and configures a unix socket and saves the filedescriptor
 *                in a connection_t object.
 *
 *  @type         global
 *
 *  @param[in]    conn  Connection object
 *
 *  @return       none
 *
 ******************************************************************************/
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

/*******************************************************************************
 *  function :    send_command_json
 ******************************************************************************/
/** @brief        Uses a initialized connection_t object to send a json string
 *                over its socket.
 *
 *  @type         global
 *
 *  @param[in]    conn     connection_t object
 *  @param[in]    command  Command string in json format
 *
 *  @return       none
 *
 ******************************************************************************/
void send_command(connection_t *conn, json_t *jsonMsg)
{
    // copy json object in stringbuffer
    strcpy(conn->command, json_getString(jsonMsg));
    // debug output
    debug(json_getString(jsonMsg));
    // send command
    send(conn->socket_fd, conn->command, STRING_SIZE, 0);
}

/*******************************************************************************
 *  function :    recieve_answer
 ******************************************************************************/
/** @brief        Recieves an answer from the ble sensor and creates a json object
 *
 *  @type         global
 *
 *  @param[in]    conn     connection_t object
 *  @param[out]   jstring  json string object
 *
 *  @return       none
 *
 ******************************************************************************/
void recieve_answer(connection_t *conn)
{
    int length = 0;
    struct timeval tv;

    tv.tv_sec = RECV_TIMEOUT_SEC;
    tv.tv_usec = 0;

    // Set socket timeout to 30s using a posix timer
    setsockopt(conn->socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

    // Read response from socket and handle timeout
     while((length = recv(conn->socket_fd, conn->answer, STRING_SIZE-1, 0) == -1)){
         debug("Socket recv timeout reached.");
         return;
     }

     conn->answer[length] = '\0'; // Append nullterminator
     printf(conn->answer);
}

/*******************************************************************************
 *  function :    sensor_connect
 ******************************************************************************/
/** @brief        Established a connection to the BLE Sensor if not already connected
 *
 *  @type         global
 *
 *  @param[in]    conn     connection_t object
 *
 *  @return       none
 *
 ******************************************************************************/
void sensor_connect(connection_t *conn, char *sensor_mac){

    // If already connected return
    if(conn->is_connected){
        return;
    }

    // create an empty json object
    json_t *jsonMsg = json_createEmpty();

    // test if object is not null
    if ( jsonMsg != NULL ) {
        // Add a key - value pair
        json_setKeyValue(jsonMsg, "device", sensor_mac);
        json_setKeyValue(jsonMsg, "command", "Connect");
    }
    // send the crafted command as string
    send_command(conn, jsonMsg);
    // recieve the answer
    recieve_answer(conn);
    // set the connected flag
    conn->is_connected = true;
    // Don ’t forget to free Json object !
    json_cleanup (jsonMsg);
}

/*******************************************************************************
 *  function :    sensor_disconnect
 ******************************************************************************/
/** @brief        Cancels a connection to the BLE sensor if a sensor is connected
 *
 *  @type         global
 *
 *  @param[in]    conn     connection_t object
 *
 *  @return       none
 *
 ******************************************************************************/
void sensor_disconnect(connection_t *conn, char *sensor_mac){
    // If not connected return
    if(!conn->is_connected){
        return;
    }

    // create an empty json object
    json_t *jsonMsg = json_createEmpty();

    // test if object is not null
    if ( jsonMsg != NULL ) {
        // Add a key - value pair
        json_setKeyValue(jsonMsg, "device", sensor_mac);
        json_setKeyValue(jsonMsg, "command", "Disconnect");
    }
    // send the crafted command as string
    send_command(conn, jsonMsg);
    // recieve the answer
    recieve_answer(conn);
    // set the connected flag to false
    conn->is_connected = false;
    // Don ’t forget to free Json object !
    json_cleanup (jsonMsg);
}

/*******************************************************************************
 *  function :    sensor_ble_scan
 ******************************************************************************/
/** @brief        Issues a BLE scan and recieves the answer from the sensor-hub
 *
 *  @type         global
 *
 *  @param[in]    conn     connection_t object
 *
 *  @return       none
 *
 ******************************************************************************/
void sensor_ble_scan(connection_t *conn)
{
    // create an empty json object
    json_t *jsonMsg = json_createEmpty();

    // test if object is not null
    if ( jsonMsg != NULL ) {
        // Add a key - value pair
        json_setKeyValue(jsonMsg, "device", "");
        json_setKeyValue(jsonMsg, "command", "StartBleScan");
    }

    // Do a BLE Scan
    send_command(conn, jsonMsg);
    // Recieve Answer
    recieve_answer(conn);
    // Don ’t forget to free Json object !
    json_cleanup (jsonMsg);
}

/*******************************************************************************
 *  function :    sensor_get_temperature
 ******************************************************************************/
/** @brief        Connects to a ble sensor, enables it's gyroscope and issues
 *                a temperature query.
 *
 *  @type         global
 *
 *  @param[in]    conn     connection_t object
 *
 *  @return       none
 *
 ******************************************************************************/
void sensor_get_temperature(connection_t *conn, char *sensor_mac)
{
    // If there is no connection abort
    if(!conn->is_connected){
        return;
    }

    // Create an empty json object
    json_t *jsonMsgGyroConfig = json_createEmpty();
    json_t *jsonSubMsgGyroConfig = json_createEmpty();
    json_t *jsonMsgTemperature = json_createEmpty();

    // ---- Gyroscope config
    if (jsonMsgGyroConfig != NULL && jsonSubMsgGyroConfig != NULL) {
        // prepare nested object
        json_setKeyValue(jsonSubMsgGyroConfig, "on", "true");
        json_setKeyValue(jsonSubMsgGyroConfig, "fullscale", "500");
        json_setKeyValue(jsonSubMsgGyroConfig, "odr", "95");
        // prepare parent object and add nested object
        json_setKeyValue(jsonMsgGyroConfig, "device", sensor_mac);
        json_setKeyValue(jsonMsgGyroConfig, "command", "ConfigAccel");
        json_insertNestedObj(jsonMsgGyroConfig, "data", jsonSubMsgGyroConfig);
    }
    // Send Gyroconfig
    send_command(conn, jsonMsgGyroConfig);

    // recieve answer
    recieve_answer(conn);

    // ---- Temperature
    if (jsonMsgTemperature != NULL) {
        // prepare json object
        json_setKeyValue(jsonMsgTemperature, "device", sensor_mac);
        json_setKeyValue(jsonMsgTemperature, "command", "GetTemperature");
    }
    // send temperature request
    send_command(conn, jsonMsgTemperature);

    // recieve answer
    //recieve_answer(conn);

    // Don ’t forget to free Json object !
    json_cleanup(jsonSubMsgGyroConfig);
    json_cleanup(jsonMsgGyroConfig);
    json_cleanup(jsonMsgTemperature);
}
