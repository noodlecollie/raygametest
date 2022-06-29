#pragma once

#include "external/cJSON_wrapper.h"

typedef struct EntityGroupDescriptor EntityGroupDescriptor;
typedef struct EntityGroupItem EntityGroupItem;

EntityGroupDescriptor* EntityGroupDescriptor_LoadFromJSONFile(const char* filePath);
EntityGroupDescriptor* EntityGroupDescriptor_LoadFromJSONObject(cJSON* root);
void EntityGroupDescriptor_Destroy(EntityGroupDescriptor* descriptor);
