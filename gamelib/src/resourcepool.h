#pragma once

#include "gamelib/external/raylibheaders.h"

struct SpriteSheetDescriptor;

typedef struct ResourcePoolTexture ResourcePoolTexture;
typedef struct ResourcePoolSpriteSheet ResourcePoolSpriteSheet;

// File paths are case sensitive here!
// Even on case-insensitive systems, they are used
// as a case-sensitive key.

ResourcePoolTexture* ResourcePool_LoadTextureAndAddRef(const char* path);
ResourcePoolTexture* ResourcePool_AddTextureRef(ResourcePoolTexture* item);
void ResourcePool_RemoveTextureRef(ResourcePoolTexture* item);
Texture2D* ResourcePool_GetTexture(ResourcePoolTexture* item);
const char* ResourcePool_GetTextureFilePath(ResourcePoolTexture* item);
