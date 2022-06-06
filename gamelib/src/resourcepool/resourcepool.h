#pragma once

#include "gamelib/external/raylibheaders.h"

struct SpriteSheetDescriptor;

typedef struct ResourcePoolTexture ResourcePoolTexture;
typedef struct ResourcePoolSpriteSheet ResourcePoolSpriteSheet;
typedef struct ResourcePoolMesh ResourcePoolMesh;
typedef struct ResourcePoolShader ResourcePoolShader;

// File paths are case sensitive here!
// Even on case-insensitive systems, they are used
// as a case-sensitive key.

ResourcePoolTexture* ResourcePool_LoadTextureAndAddRef(const char* path);
ResourcePoolTexture* ResourcePool_LoadPresetTextureAndAddRef(const char* name);
ResourcePoolTexture* ResourcePool_AddTextureRef(ResourcePoolTexture* item);
void ResourcePool_RemoveTextureRef(ResourcePoolTexture* item);
Texture2D* ResourcePool_GetTexture(ResourcePoolTexture* item);
const char* ResourcePool_GetTextureFileKey(ResourcePoolTexture* item);

ResourcePoolSpriteSheet* ResourcePool_LoadSpriteSheetAndAddRef(const char* path);
ResourcePoolSpriteSheet* ResourcePool_AddSpriteSheetRef(ResourcePoolSpriteSheet* item);
void ResourcePool_RemoveSpriteSheetRef(ResourcePoolSpriteSheet* item);
struct SpriteSheetDescriptor* ResourcePool_GetSpriteSheet(ResourcePoolSpriteSheet* item);
const char* ResourcePool_GetSpriteSheetKey(ResourcePoolSpriteSheet* item);

ResourcePoolMesh* ResourcePool_LoadPresetMeshAndAddRef(const char* name);
ResourcePoolMesh* ResourcePool_AddMeshRef(ResourcePoolMesh* item);
void ResourcePool_RemoveMeshRef(ResourcePoolMesh* item);
Mesh* ResourcePool_GetMesh(ResourcePoolMesh* item);
const char* ResourcePool_GetMeshKey(ResourcePoolMesh* item);

ResourcePoolShader* ResourcePool_LoadPresetShaderAndAddRef(const char* name);
ResourcePoolShader* ResourcePool_AddShaderRef(ResourcePoolShader* item);
void ResourcePool_RemoveShaderRef(ResourcePoolShader* item);
const char* ResourcePool_GetShaderKey(ResourcePoolShader* item);
