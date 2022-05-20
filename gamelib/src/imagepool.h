#pragma once

#include "gamelib/external/raylibheaders.h"

typedef struct ImagePoolItem ImagePoolItem;

ImagePoolItem* ImagePool_AddRef(const char* path);
void ImagePool_RemoveRef(ImagePoolItem* item);

Image* ImagePool_GetImage(ImagePoolItem* item);
