#pragma once

#include "gamelib/external/raylibheaders.h"

typedef struct ImagePoolItem ImagePoolItem;

// File paths are case sensitive here!
// Even on case-insensitive systems, they are used
// as a case-sensitive key.
ImagePoolItem* ImagePool_AddRef(const char* path);
void ImagePool_RemoveRef(ImagePoolItem* item);

Image* ImagePool_GetImage(ImagePoolItem* item);
Texture2D* ImagePool_GetTexture(ImagePoolItem* item);
const char* ImagePool_GetFilePath(ImagePoolItem* item);

// This should be called if the image itself is modified.
void ImagePool_FlagTextureNeedsUpdate(ImagePoolItem* item);
bool ImagePool_EnsureTextureUpdated(ImagePoolItem* item);
