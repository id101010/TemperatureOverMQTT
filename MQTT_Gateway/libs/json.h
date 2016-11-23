
#ifndef JSON_H_
#define JSON_H_
/******************************************************************************/
/** @file       json.h
 *******************************************************************************
 *
 *  @brief      Provides functions to communicate with json objects between
 *              client and host.
 *              <p>
 *              The module allows to create json objects out of a buffer or a
 *              string. Further an empty json object can be created. There
 *              is a function to get the string represantation of a json object,
 *              a function to add a key/value pair and a function to get a value
 *              to a corresponding key. Keep in mind, that all resource of a
 *              json object must be freed by the corresponding function.
 *              <p>
 *              Example of creating a json message out of a string:
 *              <pre>
 *              char * pcString = "{\"Hello\":\"World\"}";
 *              char * pcValue = NULL;
 *              json_t *jsonMsg = NULL;
 *
 *              jsonMsg = createJsonFromString(pcString);
 *              if(jsonMsg != NULL) {
 *                  pcValue = getJsonStringValue(jsonMsg, "Hello");
 *                  if(pcValue != NULL)
 *                      printf("%s", pcValue);
 *
 *                  // Don't forget to free ressources!
 *                  cleanUpJson(jsonMsg);
 *              }
 *              </pre>
 *              <p>
 *              Example of creating your own json message:
 *              <pre>
 *              json_t *jsonMsg = NULL;
 *              char * pcMsg = NULL;
 *              int32_t s32MsgSize = 0;
 *
 *              jsonMsg = createNewJsonMsg();
 *              if(jsonMsg != NULL) {
 *
 *                  setJsonStringKeyValue(jsonMsg, "Hello", "World");
 *                  pcMsg = getJsonMsg(jsonMsg, &s32MsgSize);
 *                  if(pcMsg != NULL) {
 *                  	printf("%s", pcMsg);
 *
 *                  	// Don't forget to free ressources!
 *                  	cleanUpSentJsonMsg(jsonMsg, pcMsg);
 *                  } else {
 *                      // Don't forget to free ressources!
 *                      cleanUpJson(jsonMsg);
 *                  }
 *              }
 *              </pre>
 *
 *  @author     wht4
 *
 ******************************************************************************/
/*
 *  function    json_createFromBuffer
 *              json_createFromString
 *              json_createEmpty
 *              json_cleanup
 *              json_getString
 *              json_freeString
 *              json_getStringValue
 *              json_setKeyValue
 *
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include <stdint.h>
#include <jansson.h>

#include "types.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
extern json_t *
json_createFromBuffer(char *pcMsg, uint32_t u32Length);

extern json_t *
json_createFromString(char *pcMsg);

extern json_t *
json_createEmpty(void);

extern Err
json_cleanup(json_t *pJson);

extern char *
json_getString(json_t * pJson);

extern void
json_freeString(char * pcJsonMsg);

extern char *
json_getStringValue(json_t * pJson, char * pcKey);

extern Err
json_setKeyValue(json_t * pJson, char * pcKey, char * pcValue);

/*----- Data -----------------------------------------------------------------*/


#endif /* JSON_H_ */
