#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include "ZScreens.h"

#ifndef MAX_ZSCREEN_JSON_SIZE
#define MAX_ZSCREEN_JSON_SIZE 1024 * 10
#endif

// Reusable Json buffer
DynamicJsonDocument zScreenDefJson(MAX_ZSCREEN_JSON_SIZE);
