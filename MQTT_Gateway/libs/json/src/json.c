/******************************************************************************/
/** @file       json.c
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
 *  @remark     Last modifications
 *                 \li August 2013, wht4, initial release
 *                 \li August 2014, wht4, adapted to new conventions
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              json_createFromBuffer
 *              json_createFromString
 *              json_createEmpty
 *              json_cleanup
 *              json_getString
 *              json_freeString
 *              json_getStringValue
 *              json_setKeyValue
 *  functions  local:
 *              .
 *
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>

#include "json.h"
#include "log.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

/*******************************************************************************
 *  function :    json_createFromBuffer
 ******************************************************************************/
/** @brief        Creates a new json object out of a character buffer.
 *                <p>
 *                The length of the json object is passed by the parameter
 *                u32Length.
 *
 *  @type         global
 *
 *  @param[in]    pcMsg     JSonMsg character buffer
 *  @param[in]    u32Length Length of the JSon message within the buffer
 *
 *  @return       Handle to the new created json object or NULL if an error
 *                occurred
 *
 ******************************************************************************/
json_t *
json_createFromBuffer(char *pcMsg, uint32_t u32Length) {

    json_t     * pJson = NULL;
    json_error_t sJSonError;

    if(pcMsg != NULL) {

        pJson = json_loadb(pcMsg, u32Length, 0, &sJSonError);
        if(pJson == NULL) {
            ERRORPRINT("Error with decoding message: %s", sJSonError.text);
        }
    }

    return (pJson);
}

/*******************************************************************************
 *  function :    json_createFromString
 ******************************************************************************/
/** @brief        Creates a new json object out of a character string.
 *                <p>
 *                The string has to be '\0' terminated.
 *
 *  @type         global
 *
 *  @param[in]    pcMsg   JSonMsg string ('\0' terminated)
 *
 *  @return       Handle to the new created json object or NULL if an error
 *                occurred
 *
 ******************************************************************************/
json_t *
json_createFromString(char *pcMsg) {

    json_t     * pJson = NULL;
    json_error_t sJSonError;

    if(pcMsg != NULL) {

        pJson = json_loads(pcMsg, 0, &sJSonError);
        if(pJson == NULL) {
            ERRORPRINT("Error with decoding message: %s", sJSonError.text);
        }
    }

    return (pJson);
}

/*******************************************************************************
 *  function :    json_createEmpty
 ******************************************************************************/
/** @brief        Creates a new empty json object.
 *                <p>
 *                This will create a new empty json object. The string
 *                representation of the object has the following form: "{}"
 *
 *  @type         global
 *
 *  @return       Handle to the new created json object or NULL if an error
 *                occurred
 *
 ******************************************************************************/
json_t *
json_createEmpty(void) {

    return(json_pack("{}"));
}

/*******************************************************************************
 *  function :    json_cleanup
 ******************************************************************************/
/** @brief        Frees the allocated resources occupied by the json object.
 *                <p>
 *                All json object created by json_createFromBuffer(),
 *                json_createFromString() or json_create occupy some
 *                resource which must be freed by calling this function
 *
 *  @type         global
 *
 *  @param[in]    pJson   json object to be freed
 *
 *  @return       <pre>
 *                BBB_SUCCESS            on success
 *                BBB_ERR_PARAM          parameter error
 *                </pre>
 *
 ******************************************************************************/
Err
json_cleanup(json_t *pJson) {

    Err error = NONE;

    if(pJson != NULL) {
        json_decref(pJson);
    } else {
        ERRORPRINT("parameter error");
        error = ERR_PARAM;
    }

    return(error);
}

/*******************************************************************************
 *  function :    json_getString
 ******************************************************************************/
/** @brief        Gets the null terminated String of the json object.
 *                <p>
 *                The returned string representation of the json object must
 *                be freed by the caller!
 *
 *  @type         global
 *
 *  @param[in]    pJson      Json message
 *
 *  @return       Json message (has to be freed after usage), or NULL on error
 *
 ******************************************************************************/
char *
json_getString(json_t * pJson) {

    char *pcJsonMsg = NULL;

    if (pJson != NULL) {

        pcJsonMsg = json_dumps(pJson, JSON_COMPACT);

    } else {
        ERRORPRINT("parameter error");
    }

    return(pcJsonMsg);
}


/*******************************************************************************
 *  function :    json_freeString
 ******************************************************************************/
/** @brief        Frees allocated ressource generated by calling json_getString()
 *                <p>
 *                If the user calls json_getString(), he get's the string
 *                representation of a corresponding json object. The string
 *                ressource must be freed! This can be done with this function
 *
 *  @type         global
 *
 *  @param[]      pcJsonMsg   Allocated string representation of a json object.
 *                            Returned by calling getStringRep()
 *
 *  @return       void
 *
 ******************************************************************************/
void
json_freeString(char * pcJsonMsg) {

    if(pcJsonMsg != NULL) {
        free(pcJsonMsg);
        pcJsonMsg = NULL;
    }
}

/*******************************************************************************
 *  function :    json_getStringValue
 ******************************************************************************/
/** @brief        Get the corresponding string value of a key out of a message.
 *                <p>
 *                Every string key within a JSon message has a corresponding
 *                string value. With this function you can access the
 *                '/0' terminated value string. This is just a borrowed
 *                reference and you are not allowed to change the value!
 *
 *  @type         global
 *
 *  @param[in]    pJson    json object
 *  @param[in]    pcKey    key string (null terminated)
 *
 *  @return       Borrowed reference to the corresponding value. You are not
 *                allowed to change the value!
 *
 ******************************************************************************/
char *
json_getStringValue(json_t * pJson, char * pcKey) {

    char   *pcValue = NULL;
    json_t *pJsonTemp = NULL;

    if((pJson != NULL) && (pcKey != NULL)) {

        pJsonTemp = json_object_get(pJson, pcKey);
        if (!json_is_string(pJsonTemp)) {
            DEBUGPRINT("Can't get value of key %s \n", pcKey);
        } else {
            pcValue = (char *) json_string_value(pJsonTemp);
        }
    }
    return (pcValue);
}

/*******************************************************************************
 *  function :    json_setKeyValue
 ******************************************************************************/
/** @brief        Sets a new key value pair within the json object.
 *                <p>
 *                If the key is already used within the message, the value will
 *                be exchanged with the new one.
 *
 *  @type         global
 *
 *  @param[in]    pJson      json object
 *  @param[in]    pcKey      new key which will be set within the message
 *  @param[in]    pcValue    value corresponding to the key
 *
 *  @return       <pre>
 *                BBB_SUCCESS       on success,
 *                BBB_ERR_PARAM     if an invalid argument was passed,
 *                BBB_JSON_PACK     if the key value pair could not be inserted
 *                </pre>
 *
 ******************************************************************************/
Err
json_setKeyValue(json_t * pJson, char * pcKey, char * pcValue) {

    Err error = NONE;

    if((pJson != NULL) && (pcKey != NULL) && (pcValue != NULL)) {

        if (json_object_set_new(pJson, pcKey, json_string(pcValue)) == -1) {

            WARNINGPRINT("Can't insert new key value pair. "
                         "Key: %s, Value: %s \n",
                         pcKey, pcValue);

            error = ERR_JSON_PACK;
        }
    } else {
        ERRORPRINT("parameter error");
        error = ERR_PARAM;
    }
    return (error);
}
