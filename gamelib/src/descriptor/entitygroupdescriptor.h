#pragma once

typedef struct EntityGroupDescriptor EntityGroupDescriptor;

EntityGroupDescriptor* EntityGroupDescriptor_LoadFromJSON(const char* filePath);
void EntityGroupDescriptor_Destroy(EntityGroupDescriptor* descriptor);
