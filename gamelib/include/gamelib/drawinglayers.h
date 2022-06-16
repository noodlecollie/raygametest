#pragma once

#define DRAWING_LAYER_LIST \
	LIST_ITEM(DLAYER_BACKGROUND = 0, "background") \
	LIST_ITEM(DLAYER_TERRAIN, "terrain") \
	LIST_ITEM(DLAYER_ITEM, "item") \
	LIST_ITEM(DLAYER_PLAYER, "player") \

#define LIST_ITEM(val, str) val,
typedef enum DrawingLayer
{
	DRAWING_LAYER_LIST

	DLAYER__COUNT,
	DLAYER__INVALID = -1
} DrawingLayer;
#undef LIST_ITEM

const char* DrawingLayer_GetName(DrawingLayer layer);
DrawingLayer DrawingLayer_GetLayerFromName(const char* name);
