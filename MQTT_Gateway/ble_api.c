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
    // Set to not connected
    conn->is_connected=false;
    // create an empty json object
    conn->jsonMsg = json_createEmpty();
}

/*******************************************************************************
 *  function :    free_connect_obj
 ******************************************************************************/
/** @brief        Deinitializes a connection object.
 *
 *  @type         global
 *
 *  @param[in]    conn     Connection object
 *
 *  @return       none
 *
 ******************************************************************************/
void free_connect_obj(connection_t *conn)
{
    // Free json object
    json_cleanup(conn->jsonMsg);
    // Close socket
    close(conn->socket_fd);
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
    // Little timeout in case there was a previous connection
    sleep(1);
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
json_t *recieve_answer(connection_t *conn)
{
    int length = 0;
    struct timeval timeout;
    timeout.tv_sec = RECV_TIMEOUT_SEC;
    timeout.tv_usec = 0;

    // Set socket timeout to 30s using a posix timer
    if (setsockopt (conn->socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
        debug("Socket option SO_RCVTIMEO failed.");
        return 0;
    }

    // Read response from socket and handle timeout
    length = recv(conn->socket_fd, conn->answer, STRING_SIZE-1, 0);

    // Ckeck for timeout
    if(length < 0){
         debug("Socket recv timeout reached.");
         return;
    }

    conn->answer[length] = '\0'; // Append nullterminator
    return json_createFromString(conn->answer);
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
bool sensor_connect(connection_t *conn, char *sensor_mac)
{
    char *pcValue = NULL;

    // If already connected return
    if(conn->is_connected){
        return false;
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
    json_t *json_answer = recieve_answer(conn);

    // get the value to the key "event"
    pcValue = json_getStringValue(json_answer, "event");

    // If device is connected
    if(strcmp(pcValue, "DeviceConnected") == 0){
        // set the connected flag
        conn->is_connected = true;
    }

    // Don ’t forget to free Json objects !
    json_freeString(pcValue);
    json_cleanup (jsonMsg);
    json_cleanup(json_answer);

    return conn->is_connected;
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
bool sensor_disconnect(connection_t *conn, char *sensor_mac)
{
    char *pcValue = NULL;

    // If not connected return
    if(!conn->is_connected){
        return false;
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
    json_t *json_answer = recieve_answer(conn);

    // get the value to the key "event"
    pcValue = json_getStringValue(json_answer, "event");

    // If device is connected
    if(strcmp(pcValue, "DeviceDisconnected") == 0){
        // set the connected flag
        conn->is_connected = false;
    }

    // Don ’t forget to free Json objects !
    //json_freeString(pcValue);
    json_cleanup(jsonMsg);

    // return true if disconnected
    return !conn->is_connected;

}

/*******************************************************************************
 *  function :    sensor_configure_gyro
 ******************************************************************************/
/** @brief        Configures the gyroscope
 *
 *  @type         global
 *
 *  @param[in]    conn     connection_t object
 *
 *  @return       none
 *
 ******************************************************************************/
bool sensor_configure_gyro(connection_t *conn, char *sensor_mac)
{
    //char *pcValue = NULL;
    //bool out = false;

    // If not connected return
    if(!conn->is_connected){
        return false;
    }

    // Create an empty json object
    json_t *jsonMsgGyroConfig = json_createEmpty();
    json_t *jsonSubMsgGyroConfig = json_createEmpty();

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
    //json_t *json_answer_configured = recieve_answer(conn);
    //debug(json_getString(json_answer_configured));

    // get the value to the key "event"
    //pcValue = json_getStringValue(json_answer_configured, "event");

    // If device is connected
    //if(strcmp(pcValue, "GyroConfigured") == 0){
    //    out = true;
    //}

    // Free ressources
    json_cleanup(jsonSubMsgGyroConfig);
    json_cleanup(jsonMsgGyroConfig);
    //json_freeString(pcValue);

    return true;
}

/*******************************************************************************
 *  function :    sensor_configure_temp
 ******************************************************************************/
/** @brief        Configures the gyroscope
 *
 *  @type         global
 *
 *  @param[in]    conn     connection_t object
 *
 *  @return       none
 *
 ******************************************************************************/
bool sensor_configure_temp(connection_t *conn, char *sensor_mac)
{
    // Create an empty json object
    json_t *jsonMsgTempStart = json_createEmpty();
    json_t *jsonSubMsgTempStart = json_createEmpty();

    // ---- Temperature sampler config
    if (jsonMsgTempStart != NULL && jsonSubMsgTempStart != NULL) {
        // prepare nested object
        json_setKeyValue(jsonSubMsgTempStart, "on", "true");
        json_setKeyValue(jsonSubMsgTempStart, "odr", "1");
        // prepare parent object and add nested object
        json_setKeyValue(jsonMsgTempStart, "device", sensor_mac);
        json_setKeyValue(jsonMsgTempStart, "command", "ConfigTemp");
        json_insertNestedObj(jsonMsgTempStart, "data", jsonSubMsgTempStart);
    }
    // Send Gyroconfig
    send_command(conn, jsonMsgTempStart);

    // Free ressources
    json_cleanup(jsonSubMsgTempStart);
    json_cleanup(jsonMsgTempStart);

    return true;
}

/*******************************************************************************
 *  function :    sensor_configure_accel
 ******************************************************************************/
/** @brief        Configures the gyroscope
 *
 *  @type         global
 *
 *  @param[in]    conn     connection_t object
 *
 *  @return       none
 *
 ******************************************************************************/
bool sensor_configure_accel(connection_t *conn, char *sensor_mac)
{
    // Create an empty json object
    json_t *jsonMsgAccelStart = json_createEmpty();
    json_t *jsonSubMsgAccelStart = json_createEmpty();

    // ---- Temperature sampler config
    if (jsonMsgAccelStart != NULL && jsonSubMsgAccelStart != NULL) {
        // prepare nested object
        json_setKeyValue(jsonSubMsgAccelStart, "on", "true");
        json_setKeyValue(jsonSubMsgAccelStart, "fullscale", "4");
        json_setKeyValue(jsonSubMsgAccelStart, "odr", "12.5");
        // prepare parent object and add nested object
        json_setKeyValue(jsonMsgAccelStart, "device", sensor_mac);
        json_setKeyValue(jsonMsgAccelStart, "command", "ConfigAccel");
        json_insertNestedObj(jsonMsgAccelStart, "data", jsonSubMsgAccelStart);
    }
    // Send Gyroconfig
    send_command(conn, jsonMsgAccelStart);

    // Free ressources
    json_cleanup(jsonSubMsgAccelStart);
    json_cleanup(jsonMsgAccelStart);

    return true;
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
void sensor_get_ble_scan(connection_t *conn)
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
    sleep(5);

    // Recieve Answer
    json_t *json_answer = recieve_answer(conn);
    debug(conn->answer); // debug print
    json_cleanup(json_answer);

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
void sensor_get_single_temperature(connection_t *conn, char *sensor_mac)
{
    char *pcValue = NULL;
    json_t *jsonMsgTemperature = json_createEmpty();

    // If there is no connection abort
    if(!conn->is_connected){
        return;
    }

    // configure the gyroscope
    sensor_configure_gyro(conn, SENSOR_MAC);

    // ---- Temperature
    if (jsonMsgTemperature != NULL) {
        // prepare json object
        json_setKeyValue(jsonMsgTemperature, "device", sensor_mac);
        json_setKeyValue(jsonMsgTemperature, "command", "GetTemperature");
    }
    // send temperature request
    send_command(conn, jsonMsgTemperature);

    // recieve answer
    json_t *json_answer_temperature = recieve_answer(conn);
    debug(json_getString(json_answer_temperature)); // debug print

    // get the value to the key "event"
    pcValue = json_getStringValue(json_answer_temperature, "data");
    debug(pcValue);

    // If device is connected
    if(pcValue != NULL){
        debug(pcValue);
    }

    // Don ’t forget to free Json objects !
    json_cleanup(jsonMsgTemperature);
    //json_cleanup(json_answer_configured);
    json_cleanup(json_answer_temperature);
}

/*******************************************************************************
 *  function :    sensor_start_temperature_sampler
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
void sensor_start_temperature_sampler(connection_t *conn, char *sensor_mac)
{
    int i = 0;

    // If there is no connection abort
    if(!conn->is_connected){
        return;
    }

    //sensor_configure_gyro(conn, sensor_mac);
    sensor_configure_temp(conn, sensor_mac);

    json_t *jsonMsgTemperatureStart = json_createEmpty();
    json_t *jsonMsgTemperatureStop = json_createEmpty();

    // prepare temperature sampling message
    if (jsonMsgTemperatureStart != NULL) {
        // prepare json object
        json_setKeyValue(jsonMsgTemperatureStart, "device", sensor_mac);
        json_setKeyValue(jsonMsgTemperatureStart, "command", "StartMeasurement");
    }

    // prepare temperature sampling stop message
    if (jsonMsgTemperatureStop != NULL) {
        // prepare json object
        json_setKeyValue(jsonMsgTemperatureStop, "device", sensor_mac);
        json_setKeyValue(jsonMsgTemperatureStop, "command", "StopMeasurement");
    }

    // start sampling
    send_command(conn, jsonMsgTemperatureStart);

    // read 50 values
    for(i = 0; i < 50; i++){
        sleep(1);
        recieve_answer(conn);
        debug(conn->answer);
    }

    // stop sampling
    send_command(conn, jsonMsgTemperatureStop);

}

/*******************************************************************************
 *  function :    sensor_start_acceleration_sampler
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
void sensor_start_acceleration_sampler(connection_t *conn, char *sensor_mac)
{
    // If there is no connection abort
    if(!conn->is_connected){
        return;
    }

    sensor_configure_gyro(conn, sensor_mac);
    sensor_configure_accel(conn, sensor_mac);

    json_t *jsonMsgAccelStart = json_createEmpty();
    json_t *jsonMsgAccelStop = json_createEmpty();

    // prepare temperature sampling message
    if (jsonMsgAccelStart != NULL) {
        // prepare json object
        json_setKeyValue(jsonMsgAccelStart, "device", sensor_mac);
        json_setKeyValue(jsonMsgAccelStart, "command", "StartMeasurement");
    }

    // prepare temperature sampling stop message
    if (jsonMsgAccelStop != NULL) {
        // prepare json object
        json_setKeyValue(jsonMsgAccelStop, "device", sensor_mac);
        json_setKeyValue(jsonMsgAccelStop, "command", "StopMeasurement");
    }

}
