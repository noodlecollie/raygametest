#pragma once

#include "gamelib/external/raylibheaders.h"

typedef struct ImagePoolItem ImagePoolItem;

// File paths are case sensitive here!
// Even on case-insensitive systems, they are used
// as a case-sensitive key.
ImagePoolItem* ImagePool_AddRef(const char* path);
void ImagePool_RemoveRef(ImagePoolItem* item);

Texture2D* ImagePool_GetTexture(ImagePoolItem* item);
const char* ImagePool_GetFilePath(ImagePoolItem* item);
