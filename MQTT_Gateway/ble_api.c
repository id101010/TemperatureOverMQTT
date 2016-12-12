/*----- Header-Files ---------------------------------------------------------*/
#include <sys/time.h>
#include <pthread.h>
#include "ble_api.h"
#include "json.h"

/*----- Defines --------------------------------------------------------------*/
#define RECV_TIMEOUT_SEC 5

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
void debug(int type, const char *msg)
{
#ifdef DEBUG
    switch(type){
        case MSG_RECV:
            printf("[RECIEVED]: \n%s\n", msg);
            break;
        case MSG_DBG:
            printf("[DEBUG]: %s\n", msg);
            break;
        case MSG_SENT:
            printf("[SENT]: \n%s\n", msg);
        case MSG_EVNT:
            printf("[EVENT]: %s\n", msg);
            break;
        default:
            printf("%s\n", msg);
        break;
    }
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
    conn->jsonMsg=json_createEmpty();
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
    debug(MSG_DBG, "Socket created.");

    // Set socket adress
    conn->socket_addr.sun_family = AF_UNIX;
    strcpy(conn->socket_addr.sun_path, SOCKET_PATH);
    conn->addr_len = strlen(conn->socket_addr.sun_path) + sizeof(conn->socket_addr.sun_family);
    debug(MSG_DBG, "Socket adress set.");

    // Connect to socket
    connect(conn->socket_fd, (struct sockaddr *)&(conn->socket_addr), (conn->addr_len));
    debug(MSG_DBG, "Socket connected.");
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
    char tmp[STRING_SIZE];
    // copy json object in stringbuffer
    strcpy(tmp, json_getString(jsonMsg));
    // debug output
    debug(MSG_SENT, json_getString(jsonMsg));
    // send command
    pthread_mutex_lock(conn->lock_send);
    send(conn->socket_fd, tmp, STRING_SIZE, 0);
    pthread_mutex_unlock(conn->lock_send);
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
bool recieve_answer(connection_t *conn, char *output)
{
    char tmp[STRING_SIZE];
    int length = 0;
    struct timeval timeout;
    timeout.tv_sec = RECV_TIMEOUT_SEC;
    timeout.tv_usec = 0;

    // Set socket timeout to 30s using a posix timer
    if (setsockopt (conn->socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
        debug(MSG_DBG, "Socket option SO_RCVTIMEO failed.");
        return 0;
    }

    // Read response from socket and handle timeout
    length = recv(conn->socket_fd, tmp, STRING_SIZE-1, 0);

    // Ckeck for timeout
    if(length < 0){
         debug(MSG_DBG, "Socket recv timeout reached.");
         return false;
    }

    tmp[length] = '\0'; // Append nullterminator
    strcpy(output, tmp);

    return true;
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
void sensor_connect(connection_t *conn, char *sensor_mac)
{
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

    // free ressources
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
void sensor_disconnect(connection_t *conn, char *sensor_mac)
{
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

    // free ressources
    json_cleanup(jsonMsg);
}

/*******************************************************************************
 *  function :    sensor_disconnect
 ******************************************************************************/
/** @brief        Forces the sensorhub to send a disconnect package
 *
 *  @type         global
 *
 *  @param[in]    conn     connection_t object
 *
 *  @return       none
 *
 ******************************************************************************/
void sensor_force_disconnect(connection_t *conn, char *sensor_mac)
{
    conn->is_connected = true;
    sensor_disconnect(conn, sensor_mac);
    conn->is_connected = false;
    debug(MSG_DBG, "Force disconnected!");
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
void sensor_configure_gyro(connection_t *conn, char *sensor_mac)
{
    // If not connected return
    if(!(conn->is_connected)){
        return;
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
        json_setKeyValue(jsonMsgGyroConfig, "command", "ConfigGyro");
        json_insertNestedObj(jsonMsgGyroConfig, "data", jsonSubMsgGyroConfig);
    }

    // Send Gyroconfig
    send_command(conn, jsonMsgGyroConfig);

    // Free ressources
    json_cleanup(jsonSubMsgGyroConfig);
    json_cleanup(jsonMsgGyroConfig);
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
void sensor_configure_temp(connection_t *conn, char *sensor_mac)
{
    // If not connected return
    if(!conn->is_connected){
        return;
    }

    // Create an empty json object
    json_t *jsonMsgTempStart = json_createEmpty();
    json_t *jsonSubMsgTempStart = json_createEmpty();

    // Assemble temperature config
    if (jsonMsgTempStart != NULL && jsonSubMsgTempStart != NULL) {
        // prepare nested object
        json_setKeyValue(jsonSubMsgTempStart, "on", "true");
        json_setKeyValue(jsonSubMsgTempStart, "odr", "1");
        // prepare parent object and add nested object
        json_setKeyValue(jsonMsgTempStart, "device", sensor_mac);
        json_setKeyValue(jsonMsgTempStart, "command", "ConfigTemp");
        json_insertNestedObj(jsonMsgTempStart, "data", jsonSubMsgTempStart);
    }

    // Send config
    send_command(conn, jsonMsgTempStart);

    // Free ressources
    json_cleanup(jsonSubMsgTempStart);
    json_cleanup(jsonMsgTempStart);
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
void sensor_configure_accel(connection_t *conn, char *sensor_mac)
{
    // If not connected return
    if(!conn->is_connected){
        return;
    }

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
    // Send config
    send_command(conn, jsonMsgAccelStart);

    // Free ressources
    json_cleanup(jsonSubMsgAccelStart);
    json_cleanup(jsonMsgAccelStart);
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
    json_t *jsonMsgStart = json_createEmpty();
    json_t *jsonMsgStop = json_createEmpty();

    // assemble start message
    if ( jsonMsgStart != NULL ) {
        // Add a key - value pair
        json_setKeyValue(jsonMsgStart, "device", "");
        json_setKeyValue(jsonMsgStart, "command", "StartBleScan");
    }

    // assemble stop message
    if ( jsonMsgStop != NULL ) {
        // Add a key - value pair
        json_setKeyValue(jsonMsgStop, "device", "");
        json_setKeyValue(jsonMsgStop, "command", "StopBleScan");
    }

    // Start the BLE scan
    send_command(conn, jsonMsgStart);

    // Scan for 5 seconds
    sleep(5);

    // Stop the BLE scan
    send_command(conn, jsonMsgStop);

    // Free ressources
    json_cleanup(jsonMsgStart);
    json_cleanup(jsonMsgStop);
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
    json_t *jsonMsgTemperature = json_createEmpty();

    // If there is no connection abort
    if(!conn->is_connected){
        return;
    }

    // configure the gyroscope
    sensor_configure_gyro(conn, SENSOR_MAC);

    // Assemble temperature request
    if (jsonMsgTemperature != NULL) {
        // prepare json object
        json_setKeyValue(jsonMsgTemperature, "device", sensor_mac);
        json_setKeyValue(jsonMsgTemperature, "command", "GetTemperature");
    }

    // send temperature request
    send_command(conn, jsonMsgTemperature);

    // free ressources
    json_cleanup(jsonMsgTemperature);
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
    // If there is no connection abort
    if(!conn->is_connected){
        return;
    }

    sensor_configure_gyro(conn, sensor_mac);
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

    // wait 20s
    sleep(20);

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
