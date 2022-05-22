#include "descriptor/spritesheetdescriptor.h"
#include "gamelib/external/raylibheaders.h"
#include "external/cJSON_wrapper.h"
#include "descriptor/descriptorutil.h"
#include "resourcepool.h"

#define SUPPORTED_VERSION 1

struct SpriteSheetDescriptor
{
	ResourcePoolTexture* frame;
};

SpriteSheetDescriptor* SpriteSheetDescriptor_LoadFromJSON(const char* filePath)
{
	if ( !filePath || !(*filePath) )
	{
		return NULL;
	}

	cJSON* root = cJSONWrapper_ParseFile(filePath);

	if ( !root )
	{
		TraceLog(LOG_ERROR, "SPRITESHEET DESCRIPTOR: Could not load %s", filePath);
		return NULL;
	}

	SpriteSheetDescriptor* descriptor = NULL;

	do
	{
		cJSON* content = Descriptor_GetContent(root, "spritesheet", SUPPORTED_VERSION);

		if ( !content )
		{
			TraceLog(LOG_ERROR, "SPRITESHEET DESCRIPTOR: File %s did not describe a version %d spritesheet", filePath, SUPPORTED_VERSION);
			break;
		}

		cJSON* animationsItem = cJSONWrapper_GetObjectItemOfType(content, "animations", cJSON_Array);
		size_t numAnimations = animationsItem ? (size_t)cJSON_GetArraySize(animationsItem) : 0;

		if ( numAnimations < 1 )
		{
			TraceLog(LOG_ERROR, "SPRITESHEET DESCRIPTOR: File %s did not contain a valid animations array", filePath);
			break;
		}

		// Until we extend support for this format:
		if ( numAnimations > 1 )
		{
			TraceLog(LOG_WARNING, "SPRITESHEET DESCRIPTOR: File %s contained %zu animations, but only 1 animation is currently supported", filePath, numAnimations);
		}

		if ( !cJSON_IsObject(animationsItem->child) )
		{
			TraceLog(LOG_ERROR, "SPRITESHEET DESCRIPTOR: File %s animation 1 was not specified as an object", filePath);
			break;
		}

		cJSON* framesItem = cJSONWrapper_GetObjectItemOfType(animationsItem->child, "frames", cJSON_Array);
		size_t numFrames = framesItem ? cJSON_GetArraySize(framesItem) : 0;

		if ( numFrames < 1 )
		{
			TraceLog(LOG_ERROR, "SPRITESHEET DESCRIPTOR: File %s animation 1 did not contain a valid frames array", filePath);
			break;
		}

		// Until we extend support for this format:
		if ( numFrames > 1 )
		{
			TraceLog(LOG_WARNING, "SPRITESHEET DESCRIPTOR: File %s animation 1 contained %zu frames, but only 1 frame is currently supported.", filePath, numFrames);
		}

		const char* frameFilePath = cJSON_GetStringValue(framesItem->child);

		if ( !frameFilePath )
		{
			TraceLog(LOG_ERROR, "SPRITESHEET DESCRIPTOR: File %s animation 1 frame 1 path was missing, or was not a string", filePath);
			break;
		}

		ResourcePoolTexture* rpTexture = ResourcePool_LoadTextureAndAddRef(frameFilePath);

		if ( !rpTexture )
		{
			TraceLog(LOG_ERROR, "SPRITESHEET DESCRIPTOR: File %s animation 1 frame 1: could not load %s", filePath, frameFilePath);
			break;
		}

		descriptor = (SpriteSheetDescriptor*)MemAlloc(sizeof(SpriteSheetDescriptor));
		descriptor->frame = rpTexture;

		TraceLog(LOG_DEBUG, "SPRITESHEET DESCRIPTOR: Loaded %s successfully", filePath);
	}
	while ( false );

	cJSON_free(root);
	return descriptor;
}

void SpriteSheetDescriptor_Destroy(SpriteSheetDescriptor* descriptor)
{
	if ( !descriptor )
	{
		return;
	}

	ResourcePool_RemoveTextureRef(descriptor->frame);
	MemFree(descriptor);
}

struct ResourcePoolTexture* SpriteSheetDescriptor_GetFrame(SpriteSheetDescriptor* descriptor)
{
	return descriptor ? descriptor->frame : NULL;
}
