#include "descriptor/spritesheetdescriptor.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gameutil.h"
#include "gamelib/gametypes.h"
#include "external/cJSON_wrapper.h"
#include "external/cJSON_util.h"
#include "descriptor/descriptorutil.h"
#include "resourcepool/resourcepool.h"
#include "listmacros.h"

#define SUPPORTED_VERSION 1
#define V1_MAX_FRAMES_PER_ANIMATION 8
#define V1_MAX_DIM_PER_FRAME 256
#define V1_DEFAULT_ANIM_FPS 3.0f

struct SpriteSheetAnimation
{
	SpriteSheetDescriptor* owner;
	struct SpriteSheetAnimation* prev;
	struct SpriteSheetAnimation* next;

	char* name;
	Texture2D texture;
	Vector2i frameBounds;
	size_t numFrames;
	float fps;
};

struct SpriteSheetDescriptor
{
	Vector2 origin;
	SpriteSheetAnimation* animListHead;
	SpriteSheetAnimation* animListTail;
	size_t numAnimations;
};

typedef struct SourceImages
{
	Image images[V1_MAX_FRAMES_PER_ANIMATION];
} SourceImages;

static void FreeAnimation(SpriteSheetAnimation* animation)
{
	if ( animation->texture.id != 0 )
	{
		UnloadTexture(animation->texture);
	}

	if ( animation->name )
	{
		MemFree(animation->name);
	}

	MemFree(animation);
}

static Vector2i LoadImagesAndCalculateMaxBounds(const char* filePath, SpriteSheetAnimation* animation, SourceImages* images, cJSON* framesArray)
{
	Vector2i bounds = { 0 };

	size_t frameIndex = 1;
	for ( cJSON* frame = framesArray->child; frame && frameIndex <= V1_MAX_FRAMES_PER_ANIMATION; frame = frame->next, ++frameIndex )
	{
		const char* frameFilePath = cJSON_GetStringValue(frame);

		if ( !frameFilePath || !(*frameFilePath) )
		{
			TraceLog(
				LOG_WARNING,
				"SPRITESHEET DESCRIPTOR: [%s] Animation \"%s\" frame %zu path was missing, or was not a valid string",
				filePath,
				animation->name,
				frameIndex
			);

			continue;
		}

		Image* image = &images->images[frameIndex - 1];
		*image = LoadImage(frameFilePath);

		if ( !image->data )
		{
			TraceLog(
				LOG_WARNING,
				"SPRITESHEET DESCRIPTOR: [%s] Animation \"%s\" frame %zu: could not load file %s",
				filePath,
				animation->name,
				frameIndex,
				frameFilePath
			);
		}

		Vector2i imgSize = (Vector2i){ image->width, image->height };

		if ( imgSize.x > V1_MAX_DIM_PER_FRAME || imgSize.y > V1_MAX_DIM_PER_FRAME )
		{
			TraceLog(
				LOG_WARNING,
				"SPRITESHEET DESCRIPTOR: [%s] Animation \"%s\" frame %zu: size of %dx%d exceeds max size of %dx%d, clamping",
				filePath,
				animation->name,
				frameIndex,
				imgSize.x,
				imgSize.y,
				V1_MAX_DIM_PER_FRAME,
				V1_MAX_DIM_PER_FRAME
			);

			if ( imgSize.x > V1_MAX_DIM_PER_FRAME )
			{
				imgSize.x = V1_MAX_DIM_PER_FRAME;
			}

			if ( imgSize.y > V1_MAX_DIM_PER_FRAME )
			{
				imgSize.y = V1_MAX_DIM_PER_FRAME;
			}
		}

		if ( imgSize.x > bounds.x )
		{
			bounds.x = imgSize.x;
		}

		if ( imgSize.y > bounds.y )
		{
			bounds.y = imgSize.y;
		}
	}

	return bounds;
}

static void UnloadSourceImages(SourceImages* images)
{
	for ( size_t index = 0; index < ARRAY_SIZE(images->images); ++index )
	{
		if ( images->images[index].data )
		{
			UnloadImage(images->images[index]);
		}
	}
}

static Image CreateCanvas(cJSON* framesItem, Vector2i bounds, const SourceImages* sourceImages, size_t numImages)
{
	Image canvas = GenImageColor((int)numImages * bounds.x, bounds.y, BLANK);

	if ( !canvas.data )
	{
		return canvas;
	}

	size_t frameIndex = 1;
	for ( cJSON* frame = framesItem->child; frame && frameIndex <= numImages; frame = frame->next, ++frameIndex )
	{
		Rectangle srcRect = (Rectangle){ 0, 0, (float)bounds.x, (float)bounds.y };
		Rectangle destRect = (Rectangle){ (float)((frameIndex - 1) * bounds.x), 0, (float)bounds.x, (float)bounds.y };
		const Image* sourceImage = &sourceImages->images[frameIndex - 1];

		if ( sourceImage->data )
		{
			if ( (int)srcRect.width > sourceImage->width )
			{
				srcRect.width = (float)sourceImage->width;
			}

			if ( (int)srcRect.height > sourceImage->height )
			{
				srcRect.height = (float)sourceImage->height;
			}

			ImageDraw(&canvas, *sourceImage, srcRect, destRect, WHITE);
		}
		else
		{
			// Make it obvious that this image failed to load.
			ImageDrawRectangleRec(&canvas, destRect, RED);
		}
	}

	return canvas;
}

static void LoadAnimation(const char* filePath, cJSON* animation, size_t index, SpriteSheetDescriptor* descriptor)
{
	if ( !cJSON_IsObject(animation) )
	{
		TraceLog(LOG_WARNING, "SPRITESHEET DESCRIPTOR: [%s] Animation %zu was not specified as an object", filePath, index);
		return;
	}

	cJSON* nameItem = cJSONWrapper_GetObjectItemOfType(animation, "name", cJSON_String);
	const char* name = nameItem ? nameItem->valuestring : NULL;

	if ( !name || !(*name) )
	{
		TraceLog(LOG_WARNING, "SPRITESHEET DESCRIPTOR: [%s] Animation %zu did not have a valid name, not loading", filePath, index);
		return;
	}

	cJSON* framesItem = cJSONWrapper_GetObjectItemOfType(animation, "frames", cJSON_Array);
	size_t numFrames = framesItem ? cJSON_GetArraySize(framesItem) : 0;

	if ( numFrames < 1 )
	{
		TraceLog(LOG_WARNING, "SPRITESHEET DESCRIPTOR: [%s] Animation \"%s\" did not contain a valid frames array, not loading", filePath, name);
		return;
	}

	if ( numFrames > V1_MAX_FRAMES_PER_ANIMATION )
	{
		TraceLog(LOG_WARNING, "SPRITESHEET DESCRIPTOR: [%s] Animation \"%s\" had %zu frames, clamping to max of %zu", filePath, name, numFrames, V1_MAX_FRAMES_PER_ANIMATION);
		numFrames = V1_MAX_FRAMES_PER_ANIMATION;
	}

	SpriteSheetAnimation* animData = (SpriteSheetAnimation*)MemAlloc(sizeof(SpriteSheetAnimation));
	DBL_LL_ADD_TO_TAIL(animData, prev, next, descriptor, animListHead, animListTail);
	++descriptor->numAnimations;

	animData->owner = descriptor;
	animData->name = DuplicateString(name);

	cJSON* fpsItem = cJSONWrapper_GetObjectItemOfType(animation, "fps", cJSON_Number);
	animData->fps = fpsItem ? (float)fpsItem->valuedouble : V1_DEFAULT_ANIM_FPS;

	TraceLog(LOG_DEBUG, "SPRITESHEET DESCRIPTOR: [%s] Animation \"%s\" has FPS of %.2f", filePath, name, animData->fps);

	SourceImages sourceImages = { 0 };
	Image canvas = { 0 };

	do
	{
		Vector2i bounds = LoadImagesAndCalculateMaxBounds(filePath, animData, &sourceImages, framesItem);

		if ( bounds.x < 1 || bounds.y < 1 )
		{
			TraceLog(LOG_WARNING, "SPRITESHEET DESCRIPTOR: [%s] Animation \"%s\": unable to load frames", filePath, name);
			break;
		}

		canvas = CreateCanvas(framesItem, bounds, &sourceImages, numFrames);

		if ( !canvas.data )
		{
			TraceLog(LOG_WARNING, "SPRITESHEET DESCRIPTOR: [%s] Animation \"%s\": unable to compile frames", filePath, name);
			break;
		}

		TraceLog(
			LOG_DEBUG,
			"SPRITESHEET DESCRIPTOR: [%s] Animation \"%s\": created intermediate canvas image of size %dx%d for %zu frames",
			filePath,
			name,
			canvas.width,
			canvas.height,
			numFrames
		);

		Texture2D texture = LoadTextureFromImage(canvas);

		if ( texture.id == 0 )
		{
			TraceLog(LOG_WARNING, "SPRITESHEET DESCRIPTOR: [%s] Animation \"%s\": unable to generate texture from frames", filePath, name);
			break;
		}

		TraceLog(
			LOG_DEBUG,
			"SPRITESHEET DESCRIPTOR: [%s] Animation \"%s\": created texture with ID %u of size %dx%d for %zu frames",
			filePath,
			name,
			texture.id,
			texture.width,
			texture.height,
			numFrames
		);

		SetTextureFilter(texture, TEXTURE_FILTER_POINT);
		SetTextureWrap(texture, TEXTURE_WRAP_CLAMP);

		animData->texture = texture;
		animData->frameBounds = bounds;
		animData->numFrames = numFrames;
	}
	while ( false );

	if ( canvas.data )
	{
		UnloadImage(canvas);
	}

	UnloadSourceImages(&sourceImages);
}

static void LoadAnimations(const char* filePath, cJSON* content, SpriteSheetDescriptor* descriptor)
{
	cJSON* animationsItem = cJSONWrapper_GetObjectItemOfType(content, "animations", cJSON_Array);

	if ( !animationsItem )
	{
		TraceLog(LOG_WARNING, "SPRITESHEET DESCRIPTOR: [%s] File did not contain a valid animations array, spritesheet will be empty", filePath);
		return;
	}

	size_t index = 1;
	for ( cJSON* item = animationsItem->child; item; item = item->next, ++index )
	{
		LoadAnimation(filePath, item, index, descriptor);
	}
}

SpriteSheetDescriptor* SpriteSheetDescriptor_LoadFromJSON(const char* filePath)
{
	if ( !filePath || !(*filePath) )
	{
		return NULL;
	}

	cJSON* root = cJSONWrapper_ParseFile(filePath);

	if ( !root )
	{
		TraceLog(LOG_ERROR, "SPRITESHEET DESCRIPTOR: [%s] Could not load file", filePath);
		return NULL;
	}

	SpriteSheetDescriptor* descriptor = NULL;
	cJSON* content = Descriptor_GetContent(root, "spritesheet", SUPPORTED_VERSION);

	if ( content )
	{
		descriptor = (SpriteSheetDescriptor*)MemAlloc(sizeof(SpriteSheetDescriptor));

		// Optional:
		cJSONUtil_GetVector2Item(content, "origin", &descriptor->origin);

		LoadAnimations(filePath, content, descriptor);

		TraceLog(LOG_DEBUG, "SPRITESHEET DESCRIPTOR: [%s] Loaded successfully (%zu animations)", filePath, descriptor->numAnimations);
	}
	else
	{
		TraceLog(LOG_ERROR, "SPRITESHEET DESCRIPTOR: [%s] File did not describe a version %d spritesheet", filePath, SUPPORTED_VERSION);
	}

	cJSON_Delete(root);
	return descriptor;
}

void SpriteSheetDescriptor_Destroy(SpriteSheetDescriptor* descriptor)
{
	if ( !descriptor )
	{
		return;
	}

	SpriteSheetAnimation* anim = descriptor->animListHead;

	while ( anim )
	{
		SpriteSheetAnimation* next = anim->next;

		FreeAnimation(anim);
		anim = next;
	}

	MemFree(descriptor);
}

Vector2 SpriteSheetDescriptor_GetOrigin(SpriteSheetDescriptor* descriptor)
{
	return descriptor ? descriptor->origin : (Vector2){ 0.0f, 0.0f };
}

SpriteSheetAnimation* SpriteSheetDescriptor_GetAnimation(SpriteSheetDescriptor* descriptor, const char* animName)
{
	if ( !descriptor || !animName || !(*animName) )
	{
		return NULL;
	}

	for ( SpriteSheetAnimation* anim = descriptor->animListHead; anim; anim = anim->next )
	{
		if ( TextIsEqual(anim->name, animName) )
		{
			return anim;
		}
	}

	return NULL;
}

SpriteSheetAnimation* SpriteSheetDescriptor_GetFirstAnimation(SpriteSheetDescriptor* descriptor)
{
	return descriptor ? descriptor->animListHead : NULL;
}

SpriteSheetAnimation* SpriteSheetDescriptor_GetNextAnimation(SpriteSheetAnimation* anim)
{
	return anim ? anim->next : NULL;
}

SpriteSheetDescriptor* SpriteSheetDescriptor_GetAnimationOwner(SpriteSheetAnimation* anim)
{
	return anim ? anim->owner : NULL;
}

Texture2D* SpriteSheetDescriptor_GetAnimationTexture(SpriteSheetAnimation* anim)
{
	return anim ? &anim->texture : NULL;
}

Vector2i SpriteSheetDescriptor_GetAnimationFrameBounds(SpriteSheetAnimation* anim)
{
	return anim ? anim->frameBounds : (Vector2i){ 0, 0 };
}

size_t SpriteSheetDescriptor_GetAnimationFrameCount(SpriteSheetAnimation* anim)
{
	return anim ? anim->numFrames : 0;
}

float SpriteSheetDescriptor_GetAnimationFPS(SpriteSheetAnimation* anim)
{
	return anim ? anim->fps : 0.0f;
}
