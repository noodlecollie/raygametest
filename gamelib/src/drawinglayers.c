#include <stddef.h>
#include "gamelib/drawinglayers.h"
#include "gamelib/gameutil.h"
#include "gamelib/external/raylibheaders.h"

#define LIST_ITEM(val, str) str,
const char* const LAYER_NAMES[DLAYER__COUNT] =
{
	DRAWING_LAYER_LIST
};
#undef LIST_ITEM

const char* DrawingLayer_GetName(DrawingLayer layer)
{
	const size_t index = (size_t)layer;
	return index < (size_t)DLAYER__COUNT ? LAYER_NAMES[index] : NULL;
}

DrawingLayer DrawingLayer_GetLayerFromName(const char* name)
{
	if ( !name )
	{
		return DLAYER__INVALID;
	}

	for ( size_t index = 0; index < ARRAY_SIZE(LAYER_NAMES); ++index )
	{
		if ( TextIsEqual(name, LAYER_NAMES[index]) )
		{
			return (DrawingLayer)index;
		}
	}

	return DLAYER__INVALID;
}
