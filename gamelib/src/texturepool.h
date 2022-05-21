#pragma once

#include "gamelib/external/raylibheaders.h"

typedef struct TexturePoolItem TexturePoolItem;

// File paths are case sensitive here!
// Even on case-insensitive systems, they are used
// as a case-sensitive key.
TexturePoolItem* TexturePool_AddRef(const char* path);
void TexturePool_RemoveRef(TexturePoolItem* item);

Texture2D* TexturePool_GetTexture(TexturePoolItem* item);
const char* TexturePool_GetFilePath(TexturePoolItem* item);
