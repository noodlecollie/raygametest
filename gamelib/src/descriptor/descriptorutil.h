#pragma once

#include <stdbool.h>
#include "external/cJSON_wrapper.h"

cJSON* Descriptor_GetContent(cJSON* root, const char* descType, int version);
