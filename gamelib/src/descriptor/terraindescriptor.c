#include "descriptor/terraindescriptor.h"
#include "descriptor/descriptorutil.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gametypes.h"
#include "gamelib/drawingdepth.h"
#include "gamelib/stringutil.h"
#include "external/cJSON_wrapper.h"
#include "external/cJSON_util.h"
#include "resourcepool/resourcepool.h"

#define SUPPORTED_VERSION 1

typedef struct TerrainLayer
{
	TerrainDescriptor* owner;

	char* name;
	size_t layerIndex;
	DrawingDepth drawingDepth;
	CollisionClass collisionClass;
	Image image;
	Texture2D texture;
} TerrainLayer;

struct TerrainDescriptor
{
	TerrainLayer* layers;
	DrawingDepth defaultDrawingDepth;
	Vector2i dimensions;
};

static inline bool DimensionsAreValid(const TerrainDescriptor* descriptor)
{
	return descriptor->dimensions.x > 0 && descriptor->dimensions.y > 0;
}

static void DeallocateLayer(TerrainLayer* layer)
{
	if ( layer->name )
	{
		MemFree(layer->name);
		layer->name = NULL;
	}

	if ( layer->texture.id != 0 )
	{
		UnloadTexture(layer->texture);
	}

	if ( layer->image.data )
	{
		UnloadImage(layer->image);
	}
}

static bool CheckLayerImageDimensions(const char* filePath, TerrainDescriptor* descriptor, TerrainLayer* layerEntry)
{
	if ( !DimensionsAreValid(descriptor) )
	{
		descriptor->dimensions.x = layerEntry->image.width;
		descriptor->dimensions.y = layerEntry->image.height;

		TraceLog(
			LOG_DEBUG,
			"TERRAIN DESCRIPTOR: [%s] Dimensions of terrain set by first encountered layer: %dx%d",
			filePath,
			descriptor->dimensions.x,
			descriptor->dimensions.y
		);

		return true;
	}

	if ( layerEntry->image.width != descriptor->dimensions.x || layerEntry->image.height != descriptor->dimensions.y )
	{
		TraceLog(
			LOG_ERROR,
			"TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" dimensions of %dx%d did not match terrain dimensions of %dx%d, not loading.",
			filePath,
			layerEntry->name ? layerEntry->name : "unnamed",
			layerEntry->image.width,
			layerEntry->image.height,
			descriptor->dimensions.x,
			descriptor->dimensions.y
		);

		return false;
	}

	return true;
}

static void LoadLayer(const char* filePath, cJSON* content, TerrainDescriptor* descriptor)
{
	TerrainLayer* layerEntry = NULL;

	do
	{
		cJSON* nameItem = cJSONWrapper_GetObjectItemOfType(content, "name", cJSON_String);
		cJSON* imageItem = cJSONWrapper_GetObjectItemOfType(content, "image", cJSON_String);
		const char* name = nameItem ? nameItem->valuestring : NULL;
		const char* logName = name ? name : "unnamed";

		if ( !imageItem || !imageItem->valuestring || !imageItem->valuestring[0] )
		{
			TraceLog(LOG_WARNING, "TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" had no image specified, skipping.", filePath, logName);
			break;
		}

		cJSON* layerIndexItem = cJSONWrapper_GetObjectItemOfType(content, "terrain_layer_index", cJSON_Number);

		if ( !layerIndexItem )
		{
			TraceLog(LOG_WARNING, "TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" had no layer index specified, skipping.", filePath, logName);
			break;
		}

		int layerIndex = layerIndexItem->valueint;

		if ( layerIndex < 0 || layerIndex >= (int)TERRAIN_MAX_LAYERS )
		{
			TraceLog(
				LOG_WARNING,
				"TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" specified layer index %d out of valid range [%d %d], skipping.",
				filePath,
				logName,
				layerIndex,
				0,
				TERRAIN_MAX_LAYERS - 1
			);

			break;
		}

		layerEntry = &descriptor->layers[layerIndex];
		layerEntry->collisionClass = COLLISIONCLASS_TERRAIN;

		if ( name && *name )
		{
			layerEntry->name = DuplicateString(name);
		}

		layerEntry->image = LoadImage(imageItem->valuestring);

		if ( !layerEntry->image.data )
		{
			TraceLog(
				LOG_ERROR,
				"TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" could not load image %s.",
				filePath,
				logName,
				imageItem->valuestring
			);

			break;
		}

		if ( !CheckLayerImageDimensions(filePath, descriptor, layerEntry) )
		{
			break;
		}

		layerEntry->texture = LoadTextureFromImage(layerEntry->image);

		if ( layerEntry->texture.id == 0 )
		{
			TraceLog(
				LOG_ERROR,
				"TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" could not create texture from image.",
				filePath,
				logName
			);

			break;
		}

		layerEntry->drawingDepth = descriptor->defaultDrawingDepth;

		cJSON* drawingDepthOverride = cJSONWrapper_GetObjectItemOfType(content, "drawing_depth", cJSON_String);

		if ( drawingDepthOverride )
		{
			DrawingDepth depth = DRAWDEPTH_DEFAULT;

			if ( DrawingDepth_FromString(drawingDepthOverride->valuestring, &depth) )
			{
				layerEntry->drawingDepth = depth;
			}
			else
			{
				TraceLog(
					LOG_WARNING,
					"TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" drawing layer \"%s\" was not recognised, ignoring.",
					filePath,
					logName,
					drawingDepthOverride->valuestring
				);
			}
		}

		cJSON* collisionClassOverride = cJSON_GetObjectItem(content, "collision_class");

		if ( collisionClassOverride )
		{
			if ( collisionClassOverride->type == cJSON_String )
			{
				CollisionClass cClass = COLLISIONCLASS_TERRAIN;

				if ( CollisionClass_FromString(collisionClassOverride->valuestring, &cClass) )
				{
					layerEntry->collisionClass = cClass;
				}
				else
				{
					TraceLog(
						LOG_WARNING,
						"TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" has unrecognised collision class override \"%s\", ignoring.",
						filePath,
						logName,
						collisionClassOverride->valuestring
					);
				}
			}
			else if ( collisionClassOverride->type == cJSON_Number )
			{
				if ( collisionClassOverride->valueint >= 0 && collisionClassOverride->valueint < (int)COLLISIONCLASS_MAX_CLASSES )
				{
					layerEntry->collisionClass = (CollisionClass)collisionClassOverride->valueint;
				}
				else
				{
					TraceLog(
						LOG_WARNING,
						"TERRAIN DESCRIPTOR: [%s] Terrain layer \"%s\" collision class override value %d was out of range [%d %d], ignoring.",
						filePath,
						logName,
						collisionClassOverride->valueint,
						0,
						COLLISIONCLASS_MAX_CLASSES - 1
					);
				}
			}
		}

		TraceLog(LOG_DEBUG, "TERRAIN DESCRIPTOR: [%s] Successfully loaded terrain layer \"%s\"", filePath, logName);

		// Success
		return;
	}
	while ( false );

	// Something went wrong.
	if ( layerEntry )
	{
		DeallocateLayer(layerEntry);
	};
}

static void LoadLayers(const char* filePath, cJSON* content, TerrainDescriptor* descriptor)
{
	cJSON* layers = cJSONWrapper_GetObjectItemOfType(content, "terrain_layers", cJSON_Array);

	if ( !layers )
	{
		TraceLog(LOG_WARNING, "TERRAIN DESCRIPTOR: [%s] Did not contain a valid terrain layers array.", filePath);
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

static TerrainDescriptor* Load(const char* filePath, cJSON* root)
{
	TerrainDescriptor* descriptor = NULL;
	cJSON* content = Descriptor_GetContent(root, "terrain", SUPPORTED_VERSION);

	if ( content )
	{
		descriptor = (TerrainDescriptor*)MemAlloc(sizeof(TerrainDescriptor));
		descriptor->layers = MemAlloc(TERRAIN_MAX_LAYERS * sizeof(TerrainLayer));
		descriptor->defaultDrawingDepth = DRAWDEPTH_TERRAIN;

		for ( size_t index = 0; index < TERRAIN_MAX_LAYERS; ++index )
		{
			descriptor->layers[index].owner = descriptor;
			descriptor->layers[index].layerIndex = index;
		}

		// Optional:
		cJSON* drawingDepth = cJSONWrapper_GetObjectItemOfType(content, "drawing_depth", cJSON_String);

		if ( drawingDepth )
		{
			DrawingDepth depth = DRAWDEPTH_DEFAULT;

			if ( DrawingDepth_FromString(drawingDepth->valuestring, &depth) )
			{
				descriptor->defaultDrawingDepth = depth;
			}
		}

		LoadLayers(filePath, content, descriptor);

		TraceLog(LOG_INFO, "TERRAIN DESCRIPTOR: [%s] Loaded successfully, dimensions %dx%d", filePath, descriptor->dimensions.x, descriptor->dimensions.y);
	}
	else
	{
		TraceLog(LOG_ERROR, "TERRAIN DESCRIPTOR: [%s] Did not describe a version %d terrain", filePath, SUPPORTED_VERSION);
	}

	return descriptor;
}

TerrainDescriptor* TerrainDescriptor_LoadFromJSONFile(const char* filePath)
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

	TerrainDescriptor* descriptor = Load(filePath, root);
	cJSON_Delete(root);

	return descriptor;
}

TerrainDescriptor* TerrainDescriptor_LoadFromJSONObject(cJSON* root)
{
	if ( !root )
	{
		return NULL;
	}

	return Load("local object", root);
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
			DeallocateLayer(&descriptor->layers[index]);
		}

		MemFree(descriptor->layers);
	}

	MemFree(descriptor);
}

Vector2i TerrainDescriptor_GetDimensionsInPixels(TerrainDescriptor* descriptor)
{
	return descriptor ? descriptor->dimensions : (Vector2i){ 0, 0 };
}

Color TerrainDescriptor_GetLayerColour(TerrainDescriptor* descriptor, size_t layer, Vector2i pos)
{
	if ( !descriptor || !DimensionsAreValid(descriptor) || layer >= TERRAIN_MAX_LAYERS || !descriptor->layers[layer].image.data )
	{
		return (Color){ 0, 0, 0, 0 };
	}

	if ( pos.x < 0 || pos.x > descriptor->dimensions.x || pos.y < 0 || pos.y > descriptor->dimensions.y )
	{
		return (Color){ 0, 0, 0, 0 };
	}

	return GetImageColor(descriptor->layers[layer].image, pos.x, pos.y);
}

Image* TerrainDescriptor_GetLayerImage(TerrainDescriptor* descriptor, size_t layer)
{
	if ( !descriptor || layer >= TERRAIN_MAX_LAYERS )
	{
		return NULL;
	}

	return &descriptor->layers[layer].image;
}

Texture2D* TerrainDescriptor_GetLayerTexture(TerrainDescriptor* descriptor, size_t layer)
{
	if ( !descriptor || layer >= TERRAIN_MAX_LAYERS )
	{
		return NULL;
	}

	return &descriptor->layers[layer].texture;
}

DrawingDepth TerrainDescriptor_GetLayerDrawingDepth(TerrainDescriptor* descriptor, size_t layer)
{
	if ( !descriptor || layer >= TERRAIN_MAX_LAYERS )
	{
		return DRAWDEPTH_TERRAIN;
	}

	return descriptor->layers[layer].drawingDepth;
}

CollisionClass TerrainDescriptor_GetLayerCollisionClass(TerrainDescriptor* descriptor, size_t layer)
{
	if ( !descriptor || layer >= TERRAIN_MAX_LAYERS )
	{
		return 0;
	}

	return descriptor->layers[layer].collisionClass;
}
