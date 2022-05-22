#pragma once

#include <stdbool.h>
#include "external/cJSON_wrapper.h"
#include "gamelib/external/raylibheaders.h"

bool cJSONUtil_IsVector2(cJSON* item);
bool cJSONUtil_GetVector2Item(cJSON* object, const char* key, Vector2* out);
