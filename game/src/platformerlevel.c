#include <stdbool.h>
#include "platformerlevel.h"
#include "gameutil.h"

#define MAX_IMAGE_WIDTH 1024
#define MAX_IMAGE_HEIGHT 256

#define LIST_ITEM(enumVal, col) col,
static const Color BLOCK_IMAGE_COLOURS[] =
{
	PLATFORMER_BLOCK_TYPE_LIST
};
#undef LIST_ITEM

static inline bool CoOrdsValid(PlatformerLevel* level, size_t x, size_t y)
{
	return level && level->data && x < level->width && y < level->height;
}

static PlatformerBlockType GetBlockTypeFromColour(Color col)
{
	for ( size_t index = 0; index < ARRAY_SIZE(BLOCK_IMAGE_COLOURS); ++index )
	{
		if ( COL_EQUAL3(col, BLOCK_IMAGE_COLOURS[index]) )
		{
			return (PlatformerBlockType)index;
		}
	}

	return BLOCK_TYPE_EMPTY;
}

void PlatformerLevel_Clear(PlatformerLevel* level)
{
	if ( !level )
	{
		return;
	}

	if ( level->data )
	{
		MemFree(level->data);
	}

	*level = (PlatformerLevel){ 0 };
}

void PlatformerLevel_LoadFromImage(PlatformerLevel* level, Image image, float scale)
{
	if ( !level )
	{
		return;
	}

	PlatformerLevel_Clear(level);
	level->scale = scale;

	if ( image.width < 1 || image.height < 1 )
	{
		return;
	}

	level->data = MemAlloc(image.width * image.height);

	if ( !level->data )
	{
		return;
	}

	level->width = MAX_OF((size_t)image.width, MAX_IMAGE_WIDTH);
	level->height = MAX_OF((size_t)image.height, MAX_IMAGE_HEIGHT);

	for ( size_t index = 0; index < level->width * level->height; ++index )
	{
		size_t x = index % level->width;
		size_t y = index / level->width;

		PlatformerBlockData* blockData = &level->data[index];
		*blockData = (PlatformerBlockData){ 0 };

		blockData->blockType = GetBlockTypeFromColour(GetImageColor(image, (int)x, (int)y));
	}
}

const PlatformerBlockData* PlatformerLevel_GetBlockByCoOrds(PlatformerLevel* level, size_t x, size_t y)
{
	if ( !CoOrdsValid(level, x, y) )
	{
		return NULL;
	}

	return &level->data[(y * level->width) + x];
}

const PlatformerBlockData* PlatformerLevel_GetBlockByPosition(PlatformerLevel* level, float x, float y)
{
	if ( !level || level->scale == 0.0f )
	{
		return NULL;
	}

	return PlatformerLevel_GetBlockByCoOrds(level, (size_t)(x / level->scale), (size_t)(y / level->scale));
}

Rectangle PlatformerLevel_GetBlockWorldRectByCoOrds(PlatformerLevel* level, size_t x, size_t y)
{
	Rectangle rect = (Rectangle){ 0 };

	if ( !level )
	{
		return rect;
	}

	rect.x = level->scale * (float)x;
	rect.y = level->scale * (float)y;
	rect.width = level->scale;
	rect.height = level->scale;

	return rect;
}
