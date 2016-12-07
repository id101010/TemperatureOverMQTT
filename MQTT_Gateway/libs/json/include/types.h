#ifndef TYPES_H_
#define TYPES_H_
/******************************************************************************/
/** @file       types.h
 *******************************************************************************
 *
 *  @brief      All data types used within the beaglebone black webhouse
 *              system.
 *              <p>
 *              Most functions will return an error of type
 *              <CODE>ERR</CODE>. All possible errors are explained
 *              in this file.
 *
 *  \author     wht4
 *
 ******************************************************************************/
/*
 *  function    .
 *
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/**
 * Error type used as a return value by most functions within the webhouse
 */
typedef enum _Err {

    NONE                  = 0,  ///< No error occurred
    ERR_UNKNOWN           = 1,  ///< A non further specified error occurred
    ERR_PARAM             = 2,  ///< A passed parameter has a wrong value, size or type

    ERR_FILE_OPEN         = 10, ///< Could not open file
    ERR_FILE_CLOSE        = 11, ///< Could not close file
    ERR_FILE_IOCTL        = 12, ///< Error in device control
    ERR_FILE_READ         = 13, ///< Error on read file

    ERR_GPIO_POLL         = 20, ///< Error on polling a gpio

    ERR_SOCKET_SOCKET     = 30, ///< Socket socket() failed
    ERR_SOCKET_OPT        = 31, ///< sock_opt() failed
    ERR_SOCKET_LISTEN     = 32, ///< Socket listen() failed
    ERR_SOCKET_BIND       = 33, ///< Socket bind() failed
    ERR_SOCKET_ACCEPT     = 34, ///< Socket accept() failed
    ERR_SOCKET_SEND       = 35, ///< Socket send() failed
    ERR_SOCKET_CLOSED     = 36, ///< Socket was closed by client
    ERR_SOCKET_NO_MSG     = 37, ///< Socket no message available on recv()

    ERR_JSON_PACK         = 40, ///< We could not pack a new json message
    ERR_JSON_CREATE       = 41, ///< We could not create json message

    ERR_RINGB_SIZE        = 50, ///< The passed buffer is to small to hold full message content
    ERR_RINGB_FULL        = 51, ///< You try to write into a ringbuffer which is full
    ERR_RINGB_EMPTY       = 52, ///< You try to read from a ringbuffer which is empty

    ERR_BLOCK_SIGNAL      = 60, ///< We could not block signals for the calling thread
    ERR_SIGNAL_HANDLER    = 61, ///< Could not register callback function

    ERR_THREAD_RUNNING    = 70, ///< The thread is allready running
    ERR_THREAD_CREATE     = 71, ///< The thread could not be created

    ERR_CMD_INVALID       = 80, ///< An invalid cmd object was received
    ERR_CMD_NO_CMD        = 81, ///< No cmd object is available
    ERR_CMD_COMPOSE_MSG   = 82  ///< The response could not be composed

} Err;

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/


#endif /* TYPES_H_ */
