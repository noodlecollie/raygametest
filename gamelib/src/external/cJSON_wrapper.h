#pragma once

#include "cJSON/cJSON.h"

// This will be called to point the cJSON malloc/free functions to our own checked functions
// if any of the parsing functions in this file are used. However, if you use cJSON functions
// directly, you will need to call this function yourself at least once before doing any parsing.
void cJSONWrapper_InitHooks(void);

cJSON* cJSONWrapper_ParseFile(const char* filePath);

cJSON* cJSONWrapper_GetObjectItemOfType(cJSON* object, const char* key, int itemType);
