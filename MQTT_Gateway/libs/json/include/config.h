#ifndef CONFIG_H_
#define CONFIG_H_
/******************************************************************************/
/** @file       config.h
 *******************************************************************************
 *
 *  @brief      All configurations of the beaglebone black webhouse are handled
 *              within this file.
 *
 *  @author     wht4
 *
 ******************************************************************************/
/*
 *  function    .
 *
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/

/*----- Macros ---------------------------------------------------------------*/

/*******************************************************************************
 * System defines. Used for system information
 ******************************************************************************/
/* Version of the beaglebone black webhouse. Consists of the underlying OS    */
/* followed by a major and minor version number OS_x.y                        */
#define CONFIG_BBB_VERSION                  ( "DebianJessie" )
#define CONFIG_BBB_BUILD_DATE               ( __DATE__ )
#define CONFIG_BBB_BUILD_TIME               ( __TIME__ )
#define CONFIG_BBB_NAME                     ( "beaglebone black webhouse" )

/*******************************************************************************
 *  Log mechanism configuration
 ******************************************************************************/
/* Set CONFIG_LOG_CONSOLE to zero if you don't wan't any console output of    */
/* the logging mechanism. Further the log messages are grouped in different   */
/* levels. Just set a log level to zero and all messages of this level will   */
/* not appear.                                                                */
#define CONFIG_LOG_CONSOLE                  ( 1 )
#define CONFIG_LOG_LEVEL_DEBUG              ( 1 )
#define CONFIG_LOG_LEVEL_INFO               ( 1 )
#define CONFIG_LOG_LEVEL_WARNING            ( 1 )
#define CONFIG_LOG_LEVEL_ERROR              ( 1 )

/*******************************************************************************
 *  Socket interface configuration
 ******************************************************************************/
/* Socket communication interface constants. You can set the socket port you  */
/* wan't to listen at and the buffer size for incoming data                   */
#define CONFIG_SOCKET_PORT                  ( 5000 )
#define CONFIG_SOCKET_INPUT_BUFFER          ( 128 )

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/


#endif /* CONFIG_H_ */
