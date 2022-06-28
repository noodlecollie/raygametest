#pragma once

typedef struct EntityGroupDescriptor EntityGroupDescriptor;

EntityGroupDescriptor* EntityGroupDescriptor_LoadFromJSONFile(const char* filePath);
void EntityGroupDescriptor_Destroy(EntityGroupDescriptor* descriptor);
