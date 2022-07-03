#pragma once

#include "gamelib/external/raylibheaders.h"

struct cJSON;
struct SpriteSheetDescriptor;
struct TerrainDescriptor;

typedef struct ResourcePoolTexture ResourcePoolTexture;
typedef struct ResourcePoolSpriteSheet ResourcePoolSpriteSheet;
typedef struct ResourcePoolTerrain ResourcePoolTerrain;
typedef struct ResourcePoolMesh ResourcePoolMesh;
typedef struct ResourcePoolShader ResourcePoolShader;

// File paths are case sensitive here!
// Even on case-insensitive systems, they are used
// as a case-sensitive key.

// Modifying the pools themselves (ie. adding/removing refs)
// is thread-safe, but accessing the contents of a resource is not.
// This is to facilitate contents being asynchronously loaded.
// Contents of resources should not be shared between threads
// unless other thread protection is in place.

ResourcePoolTexture* ResourcePool_LoadTextureFromFileAndAddRef(const char* path);
ResourcePoolTexture* ResourcePool_LoadPresetTextureAndAddRef(const char* name);
ResourcePoolTexture* ResourcePool_AddTextureRef(ResourcePoolTexture* item);
void ResourcePool_RemoveTextureRef(ResourcePoolTexture* item);
Texture2D* ResourcePool_GetTexture(ResourcePoolTexture* item);
const char* ResourcePool_GetTextureFileKey(ResourcePoolTexture* item);

ResourcePoolSpriteSheet* ResourcePool_LoadSpriteSheetFromFileAndAddRef(const char* path);
ResourcePoolSpriteSheet* ResourcePool_AddSpriteSheetRef(ResourcePoolSpriteSheet* item);
void ResourcePool_RemoveSpriteSheetRef(ResourcePoolSpriteSheet* item);
struct SpriteSheetDescriptor* ResourcePool_GetSpriteSheet(ResourcePoolSpriteSheet* item);
const char* ResourcePool_GetSpriteSheetKey(ResourcePoolSpriteSheet* item);

ResourcePoolTerrain* ResourcePool_LoadTerrainFromFileAndAddRef(const char* path);
ResourcePoolTerrain* ResourcePool_LoadTerrainFromJSONAndAddRef(const char* key, struct cJSON* root);
ResourcePoolTerrain* ResourcePool_AddTerrainRef(ResourcePoolTerrain* item);
void ResourcePool_RemoveTerrainRef(ResourcePoolTerrain* item);
struct TerrainDescriptor* ResourcePool_GetTerrain(ResourcePoolTerrain* item);
const char* ResourcePool_GetTerrainKey(ResourcePoolTerrain* item);

ResourcePoolMesh* ResourcePool_LoadPresetMeshAndAddRef(const char* name);
ResourcePoolMesh* ResourcePool_AddMeshRef(ResourcePoolMesh* item);
void ResourcePool_RemoveMeshRef(ResourcePoolMesh* item);
Mesh* ResourcePool_GetMesh(ResourcePoolMesh* item);
const char* ResourcePool_GetMeshKey(ResourcePoolMesh* item);

ResourcePoolShader* ResourcePool_LoadPresetShaderAndAddRef(const char* name);
ResourcePoolShader* ResourcePool_AddShaderRef(ResourcePoolShader* item);
void ResourcePool_RemoveShaderRef(ResourcePoolShader* item);
Shader* ResourcePool_GetShader(ResourcePoolShader* item);
const char* ResourcePool_GetShaderKey(ResourcePoolShader* item);
