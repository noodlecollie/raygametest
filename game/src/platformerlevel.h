#pragma once

#include <stddef.h>
#include "raylib.h"

#define PLATFORMER_BLOCK_TYPE_LIST \
	LIST_ITEM(BLOCK_TYPE_EMPTY = 0, ((Color){ 255, 255, 255, 255 })) \
	LIST_ITEM(BLOCK_TYPE_GROUND, ((Color){ 0, 0, 0, 255 }))

#define LIST_ITEM(enumVal, col) enumVal,
typedef enum PlatformerBlockType
{
	PLATFORMER_BLOCK_TYPE_LIST
} PlatformerBlockType;
#undef LIST_ITEM

typedef struct PlatformerBlockData
{
	PlatformerBlockType blockType;
} PlatformerBlockData;

typedef struct PlatformerLevel
{
	size_t width;
	size_t height;
	PlatformerBlockData* data;

	// World units per block
	float scale;
} PlatformerLevel;

void PlatformerLevel_Clear(PlatformerLevel* level);
void PlatformerLevel_LoadFromImage(PlatformerLevel* level, Image image, float scale);
const PlatformerBlockData* PlatformerLevel_GetBlockDataByCoOrds(PlatformerLevel* level, size_t x, size_t y);
const PlatformerBlockData* PlatformerLevel_GetBlockDataByPosition(PlatformerLevel* level, float x, float y);
Rectangle PlatformerLevel_GetBlockWorldRectByCoOrds(PlatformerLevel* level, size_t x, size_t y);
