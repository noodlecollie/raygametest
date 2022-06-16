#include "descriptor/terraindescriptor.h"
#include "descriptor/descriptorutil.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gametypes.h"
#include "gamelib/drawinglayers.h"
#include "external/cJSON_wrapper.h"
#include "external/cJSON_util.h"
#include "resourcepool/resourcepool.h"

#define SUPPORTED_VERSION 1
#define TERRAIN_MAX_LAYERS MASK32_BITS

typedef struct TerrainLayer
{
	ResourcePoolTexture* imageResource;
} TerrainLayer;

struct TerrainDescriptor
{
	TerrainLayer* layers;
	DrawingLayer defaultDrawingLayer;
};

static void DestroyLayer(TerrainLayer* layer)
{
	if ( layer->imageResource )
	{
		ResourcePool_RemoveTextureRef(layer->imageResource);
		layer->imageResource = NULL;
	}
}

static void LoadLayer(const char* filePath, cJSON* content, TerrainDescriptor* descriptor)
{
	cJSON* nameItem = cJSONWrapper_GetObjectItemOfType(content, "name", cJSON_String);
	const char* name = nameItem ? nameItem->valuestring : NULL;

	cJSON* imageItem = cJSONWrapper_GetObjectItemOfType(content, "image", cJSON_String);

	if ( !imageItem || !imageItem->valuestring || !imageItem->valuestring[0] )
	{
		TraceLog(LOG_WARNING, "TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" had no image specified, skipping.", filePath, name ? name : "unnamed");
		return;
	}

	cJSON* layerIndexItem = cJSONWrapper_GetObjectItemOfType(content, "terrain_layer_index", cJSON_Number);

	if ( !layerIndexItem )
	{
		TraceLog(LOG_WARNING, "TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" had no layer index specified, skipping.", filePath, name ? name : "unnamed");
		return;
	}

	int layerIndex = layerIndexItem->valueint;

	if ( layerIndex < 0 || layerIndex >= (int)TERRAIN_MAX_LAYERS )
	{
		TraceLog(
			LOG_WARNING,
			"TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" specified layer index %d out of valid range [%d %d], skipping.",
			filePath,
			name ? name : "unnamed",
			layerIndex,
			0,
			TERRAIN_MAX_LAYERS - 1
		);

		return;
	}

	TerrainLayer* layerEntry = &descriptor->layers[layerIndex];

	layerEntry->imageResource = ResourcePool_LoadTextureAndAddRef(imageItem->valuestring);

	if ( !layerEntry->imageResource )
	{
		TraceLog(
			LOG_ERROR,
			"TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" could not load image %s.",
			filePath,
			name ? name : "unnamed",
			imageItem->valuestring
		);

		return;
	}

	TraceLog(LOG_DEBUG, "TERRAIN DESCRIPTOR: [%s] Successfully loaded terrain layer \"%s\"", filePath, name ? name : "unnamed");
}

static void LoadLayers(const char* filePath, cJSON* content, TerrainDescriptor* descriptor)
{
	cJSON* layers = cJSONWrapper_GetObjectItemOfType(content, "terrain_layers", cJSON_Array);

	if ( !layers )
	{
		TraceLog(LOG_WARNING, "TERRAIN DESCRIPTOR: [%s] File did not contain a valid terrain layers array.", filePath);
		return;
	}

	size_t index = 0;
	for ( cJSON* child = layers->child; child; child = child->next, ++index )
	{
		if ( child->type != cJSON_Object )
		{
			TraceLog(LOG_WARNING, "TERRAIN DESCRIPTOR: [%s] Terrain layer %zu was not specified as an object.", filePath, index + 1);
			continue;
		}

		LoadLayer(filePath, child, descriptor);
	}
}

TerrainDescriptor* TerrainDescriptor_LoadFromJSON(const char* filePath)
{
	if ( !filePath || !(*filePath) )
	{
		return NULL;
	}

	cJSON* root = cJSONWrapper_ParseFile(filePath);

	if ( !root )
	{
		TraceLog(LOG_ERROR, "TERRAIN DESCRIPTOR: [%s] Could not load file", filePath);
		return NULL;
	}

	TerrainDescriptor* descriptor = NULL;
	cJSON* content = Descriptor_GetContent(root, "terrain", SUPPORTED_VERSION);

	if ( content )
	{
		descriptor = (TerrainDescriptor*)MemAlloc(sizeof(TerrainDescriptor));
		descriptor->layers = MemAlloc(TERRAIN_MAX_LAYERS * sizeof(TerrainLayer));
		descriptor->defaultDrawingLayer = DLAYER_TERRAIN;

		// Optional:
		cJSON* drawingLayer = cJSONWrapper_GetObjectItemOfType(content, "drawing_layer", cJSON_String);

		if ( drawingLayer && drawingLayer->valuestring )
		{
			DrawingLayer dLayer = DrawingLayer_GetLayerFromName(drawingLayer->valuestring);

			if ( dLayer != DLAYER__INVALID )
			{
				descriptor->defaultDrawingLayer = dLayer;
			}
		}

		LoadLayers(filePath, content, descriptor);

		TraceLog(LOG_INFO, "TERRAIN DESCRIPTOR: [%s] Loaded successfully", filePath);
	}
	else
	{
		TraceLog(LOG_ERROR, "TERRAIN DESCRIPTOR: [%s] File did not describe a version %d terrain", filePath, SUPPORTED_VERSION);
	}

	cJSON_Delete(root);
	return descriptor;
}

void TerrainDescriptor_Destroy(TerrainDescriptor* descriptor)
{
	if ( !descriptor )
	{
		return;
	}

	if ( descriptor->layers )
	{
		for ( size_t index = 0; index < TERRAIN_MAX_LAYERS; ++index )
		{
			DestroyLayer(&descriptor->layers[index]);
		}

		MemFree(descriptor->layers);
	}

	MemFree(descriptor);
}
