#pragma once

#include "gamelib/external/raylibheaders.h"
#include "presetnames.h"

struct SpriteSheetDescriptor;

typedef struct ResourcePoolTexture ResourcePoolTexture;
typedef struct ResourcePoolSpriteSheet ResourcePoolSpriteSheet;
typedef struct ResourcePoolMesh ResourcePoolMesh;

// File paths are case sensitive here!
// Even on case-insensitive systems, they are used
// as a case-sensitive key.

ResourcePoolTexture* ResourcePool_LoadTextureAndAddRef(const char* path);
ResourcePoolTexture* ResourcePool_AddTextureRef(ResourcePoolTexture* item);
void ResourcePool_RemoveTextureRef(ResourcePoolTexture* item);
Texture2D* ResourcePool_GetTexture(ResourcePoolTexture* item);
const char* ResourcePool_GetTextureFilePath(ResourcePoolTexture* item);

ResourcePoolSpriteSheet* ResourcePool_LoadSpriteSheetAndAddRef(const char* path);
ResourcePoolSpriteSheet* ResourcePool_AddSpriteSheetRef(ResourcePoolSpriteSheet* item);
void ResourcePool_RemoveSpriteSheetRef(ResourcePoolSpriteSheet* item);
struct SpriteSheetDescriptor* ResourcePool_GetSpriteSheet(ResourcePoolSpriteSheet* item);
const char* ResourcePool_GetSpriteSheetFilePath(ResourcePoolSpriteSheet* item);

ResourcePoolMesh* ResourcePool_AddMeshRef(const char* name);
void ResourcePool_RemoveMeshRef(ResourcePoolMesh* item);
Mesh* ResourcePool_GetMesh(ResourcePoolMesh* item);
const char* ResourcePool_GetMeshPresetName(ResourcePoolMesh* item);
