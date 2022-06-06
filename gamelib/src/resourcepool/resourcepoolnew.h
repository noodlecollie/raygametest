#pragma once

#include "gamelib/external/raylibheaders.h"

struct SpriteSheetDescriptor;

typedef struct ResourcePoolTextureNew ResourcePoolTextureNew;
typedef struct ResourcePoolSpriteSheetNew ResourcePoolSpriteSheetNew;
typedef struct ResourcePoolMeshNew ResourcePoolMeshNew;

// File paths are case sensitive here!
// Even on case-insensitive systems, they are used
// as a case-sensitive key.

ResourcePoolTextureNew* ResourcePoolNew_LoadTextureAndAddRef(const char* path);
ResourcePoolTextureNew* ResourcePoolNew_LoadPresetTextureAndAddRef(const char* name);
ResourcePoolTextureNew* ResourcePoolNew_AddTextureRef(ResourcePoolTextureNew* item);
void ResourcePoolNew_RemoveTextureRef(ResourcePoolTextureNew* item);
Texture2D* ResourcePoolNew_GetTexture(ResourcePoolTextureNew* item);
const char* ResourcePoolNew_GetTextureFileKey(ResourcePoolTextureNew* item);

ResourcePoolSpriteSheetNew* ResourcePoolNew_LoadSpriteSheetAndAddRef(const char* path);
ResourcePoolSpriteSheetNew* ResourcePoolNew_AddSpriteSheetRef(ResourcePoolSpriteSheetNew* item);
void ResourcePoolNew_RemoveSpriteSheetRef(ResourcePoolSpriteSheetNew* item);
struct SpriteSheetDescriptor* ResourcePoolNew_GetSpriteSheet(ResourcePoolSpriteSheetNew* item);
const char* ResourcePoolNew_GetSpriteSheetKey(ResourcePoolSpriteSheetNew* item);

ResourcePoolMeshNew* ResourcePoolNew_LoadPresetMeshAndAddRef(const char* name);
ResourcePoolMeshNew* ResourcePoolNew_AddMeshRef(ResourcePoolMeshNew* item);
void ResourcePoolNew_RemoveMeshRef(ResourcePoolMeshNew* item);
Mesh* ResourcePoolNew_GetMesh(ResourcePoolMeshNew* item);
const char* ResourcePoolNew_GetMeshKey(ResourcePoolMeshNew* item);
