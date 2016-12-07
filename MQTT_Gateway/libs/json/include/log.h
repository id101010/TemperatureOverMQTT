#ifndef LOG_H_
#define LOG_H_
/******************************************************************************/
/** @file       log.h
 *******************************************************************************
 *
 *  @brief      Log mechanism of the beaglebone black webhouse
 *              <p>
 *              Within the webhouse the objects log different
 *              messages to this log mechanism. The log messages are grouped
 *              in four different levels (DEBUG, INFO, WARNING and ERROR).
 *              <p>
 *              The log messages can be bypassed to different acceptors
 *              (console, file, ...).
 *              <p>
 *              If you like to log a message to the log mechanism, you have
 *              to do the following:
 *              <ul>
 *              <li> Debug messages are logged with: DEBUGPRINT("msg");
 *              <li> Info messages are logged with: INFOPRINT("msg");
 *              <li> Warning messages are logged with: WARNINGPRINT("msg");
 *              <li> Error messages are logged with: ERRORPRINT("msg");
 *              </ul>
 *
 *  @author     author
 *
 ******************************************************************************/
/*
 *  function    .
 *
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "config.h"

#if (CONFIG_LOG_CONSOLE == 1)
#define LOG_CONSOLE
#include "logConsole.h"
#endif // (CONFIG_LOG_CONSOLE == 1)

/*----- Macros ---------------------------------------------------------------*/
#if (CONFIG_LOG_LEVEL_DEBUG == 1)
#define LOG_DEBUG
#endif // (CONFIG_LOG_LEVEL_DEBUG == 1)

#if (CONFIG_LOG_LEVEL_INFO == 1)
#define LOG_INFO
#endif // (CONFIG_LOG_LEVEL_INFO == 1)

#if (CONFIG_LOG_LEVEL_WARNING == 1)
#define LOG_WARNING
#endif // (CONFIG_LOG_LEVEL_WARNING == 1)

#if (CONFIG_LOG_LEVEL_ERROR == 1)
#define LOG_ERROR
#endif // (CONFIG_LOG_LEVEL_ERROR == 1)


/*******************************************************************************
 *  function :    LOGINIT
 ******************************************************************************/
/**
 * Initialize the log mechanism
 * @return      void
 */
#define LOGINIT() {                                                             \
		INIT_CONSOLE();                                                         \
}


/*******************************************************************************
 *  function :    DEBUGPRINT
 ******************************************************************************/
/**
 * Log a debug message
 * @param[in]   FormatString   String that contains the text to be written
 * @param[in]   Args           Depending on the format string, the function
 *                             may expect a sequence of additional arguments
 * @return      void
 */
#ifdef LOG_DEBUG
#define DEBUGPRINT(FormatString, Args...) {                                     \
	DEBUGPRINT_CONSOLE(FormatString, ##Args)                                    \
}
#else
#define DEBUGPRINT(...)
#endif // #ifdef LOG_DEBUG


/*******************************************************************************
 *  function :    INFOPRINT
 ******************************************************************************/
/**
 * Log a info message
 * @param[in]   FormatString   String that contains the text to be written
 * @param[in]   Args           Depending on the format string, the function
 *                             may expect a sequence of additional arguments
 * @return      void
 */
#ifdef LOG_INFO
#define INFOPRINT(FormatString, Args...) {                                     \
	INFOPRINT_CONSOLE(FormatString, ##Args)                                    \
}
#else
#define INFOPRINT(...)
#endif // #ifdef LOG_DEBUG


/*******************************************************************************
 *  function :    WARNINGPRINT
 ******************************************************************************/
/**
 * Log a warning message
 * @param[in]   FormatString   String that contains the text to be written
 * @param[in]   Args           Depending on the format string, the function
 *                             may expect a sequence of additional arguments
 * @return      void
 */
#ifdef LOG_WARNING
#define WARNINGPRINT(FormatString, Args...) {                                 \
	WARNINGPRINT_CONSOLE(FormatString, ##Args)                                \
}
#else
#define WARNINGRINT(...)
#endif // #ifdef LOG_DEBUG


/*******************************************************************************
 *  function :    ERRORPRINT
 ******************************************************************************/
/**
 * Log an error message
 * @param[in]   FormatString   String that contains the text to be written
 * @param[in]   Args           Depending on the format string, the function
 *                             may expect a sequence of additional arguments
 * @return      void
 */
#ifdef LOG_ERROR
#define ERRORPRINT(FormatString, Args...) {                                   \
	ERRORPRINT_CONSOLE(FormatString, ##Args)                                  \
}
#else
#define ERRORPRINT(...)
#endif // #ifdef LOG_DEBUG

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/


#endif /* LOG_H_ */
