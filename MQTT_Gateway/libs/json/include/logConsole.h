#ifndef LOGCONSOLE_H_
#define LOGCONSOLE_H_
/******************************************************************************/
/** @file       LogConsole.h
 *******************************************************************************
 *
 *  @brief      Acceptor of log message which logs the messages to the stdout.
 *              <p>
 *              The log messages are grouped in four different levels
 *              (DEBUG, INFO, WARNING and ERROR).
 *              <p>
 *              The log messages are bypassed to different acceptors
 *              (console, file, ...). This is an acceptor which puts the
 *              messages to stdout.
 *
 *  @author     wht4
 *
 ******************************************************************************/
/*
 *  function    .
 *
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include <stdio.h>

/*----- Macros ---------------------------------------------------------------*/

/*******************************************************************************
 *  function :    INIT_CONSOLE
 ******************************************************************************/
/**
 * Initialize the log acceptor console
 * @return      void
 */
#ifdef LOG_CONSOLE
#define INIT_CONSOLE() {                                                       \
	setvbuf (stdout, NULL, _IONBF, 0);                                         \
}


/*******************************************************************************
 *  function :    DEBUGPRINT_CONSOLE
 ******************************************************************************/
/**
 * Log a debug message to stdout
 * @param[in]   FormatString   String that contains the text to be written
 * @param[in]   Args           Depending on the format string, the function
 *                             may expect a sequence of additional arguments
 * @return      void
 */
#define DEBUGPRINT_CONSOLE(FormatString, Args...) {                            \
	flockfile(stdout);                                                         \
	printf("\nDEBUG:  \t%s;\n\t\t",__FUNCTION__);                              \
	printf(FormatString, ##Args);                                              \
	fflush(stdout);                                                            \
	funlockfile(stdout);                                                       \
}
#else
#define DEBUGPRINT_CONSOLE(...)
#endif // #ifdef LOG_CONSOLE


/*******************************************************************************
 *  function :    INFOPRINT_CONSOLE
 ******************************************************************************/
/**
 * Log an info message to stdout
 * @param[in]   FormatString   String that contains the text to be written
 * @param[in]   Args           Depending on the format string, the function
 *                             may expect a sequence of additional arguments
 * @return      void
 */
#ifdef LOG_CONSOLE
#define INFOPRINT_CONSOLE(FormatString, Args...) {                             \
	flockfile(stdout);                                                         \
	printf("\nINFO:   \t%s;\n\t\t",__FUNCTION__);                              \
	printf(FormatString, ##Args);                                              \
	fflush(stdout);                                                            \
	funlockfile(stdout);                                                       \
}
#else
#define INFOPRINT_CONSOLE(...)
#endif // #ifdef LOG_CONSOLE


/*******************************************************************************
 *  function :    WARNINGPRINT_CONSOLE
 ******************************************************************************/
/**
 * Log a warning message to stdout
 * @param[in]   FormatString   String that contains the text to be written
 * @param[in]   Args           Depending on the format string, the function
 *                             may expect a sequence of additional arguments
 * @return      void
 */
#ifdef LOG_CONSOLE
#define WARNINGPRINT_CONSOLE(FormatString, Args...) {                          \
	flockfile(stdout);                                                         \
	printf("\nWARNING:\t%s;\n\t\t",__FUNCTION__);                              \
	printf(FormatString, ##Args);                                              \
	fflush(stdout);                                                            \
	funlockfile(stdout);                                                       \
}
#else
#define WARNINGRINT_CONSOLE(...)
#endif // #ifdef LOG_CONSOLE


/*******************************************************************************
 *  function :    ERRORPRINT_CONSOLE
 ******************************************************************************/
/**
 * Log an error message to stdout
 * @param[in]   FormatString   String that contains the text to be written
 * @param[in]   Args           Depending on the format string, the function
 *                             may expect a sequence of additional arguments
 * @return      void
 */
#ifdef LOG_CONSOLE
#define ERRORPRINT_CONSOLE(FormatString, Args...) {                            \
	flockfile(stdout);                                                         \
	printf("\nERROR:  \t%s;\n\t\t",__FUNCTION__);                              \
	printf(FormatString, ##Args);                                              \
	fflush(stdout);                                                            \
	funlockfile(stdout);                                                       \
}
#else
#define ERRORPRINT_CONSOLE(...)
#endif // #ifdef LOG_CONSOLE

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/


#endif /* LOGCONSOLE_H_ */
