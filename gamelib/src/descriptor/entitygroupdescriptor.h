#pragma once

#include "external/cJSON_wrapper.h"

typedef struct EntityGroupDescriptor EntityGroupDescriptor;
typedef struct EntityGroupItem EntityGroupItem;

struct ResourcePoolTerrain;
struct ResourcePoolSpriteSheet;

EntityGroupDescriptor* EntityGroupDescriptor_LoadFromJSONFile(const char* filePath);
EntityGroupDescriptor* EntityGroupDescriptor_LoadFromJSONObject(cJSON* root);
void EntityGroupDescriptor_Destroy(EntityGroupDescriptor* descriptor);
const char* EntityGroupDescriptor_GetName(EntityGroupDescriptor* descriptor);

size_t EntityGroupDescriptor_GetItemCount(const EntityGroupDescriptor* descriptor);
EntityGroupItem* EntityGroupDescriptor_GetFirstItem(EntityGroupDescriptor* descriptor);
EntityGroupItem* EntityGroupDescriptor_GetPreviousItem(EntityGroupItem* item);
EntityGroupItem* EntityGroupDescriptor_GetNextItem(EntityGroupItem* item);

const char* EntityGroupDescriptor_GetItemName(EntityGroupItem* item);
size_t EntityGroupDescriptor_GetItemIndex(EntityGroupItem* item);
size_t EntityGroupDescriptor_GetItemTagCount(EntityGroupItem* item);
const char* EntityGroupDescriptor_GetItemTag(EntityGroupItem* item, size_t index);

struct ResourcePoolTerrain* EntityGroupDescriptor_GetTerrainResource(EntityGroupItem* item);
struct ResourcePoolSpriteSheet* EntityGroupDescriptor_GetSpriteSheetResource(EntityGroupItem* item);
