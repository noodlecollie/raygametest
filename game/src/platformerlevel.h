#pragma once

#include <stddef.h>
#include "raylib.h"
#include "gametypes.h"

#define PLATFORMERLEVEL_MAX_LAYERS 32

struct PlatformerLevelLayer;

typedef struct PlatformerLevel
{
	struct PlatformerLevelLayer* layers;

	// World units per block
	float scale;
} PlatformerLevel;

void PlatformerLevel_Unload(PlatformerLevel level);

void PlatformerLevel_LoadLayer(PlatformerLevel* level, size_t layer, const char* fileName);
void PlatformerLevel_UnloadLayer(PlatformerLevel* level, size_t layer);
Vector2i PlatformerLevel_GetLayerDimensions(PlatformerLevel level, size_t layer);

Color PlatformerLevel_GetBlockColourByCoOrds(PlatformerLevel level, size_t layer, Vector2i coOrds);
Rectangle PlatformerLevel_GetBlockWorldRectByCoOrds(PlatformerLevel level, Vector2i coOrds);

Vector2i PlatformerLevel_PositionToCoOrds(PlatformerLevel level, Vector2 world);
