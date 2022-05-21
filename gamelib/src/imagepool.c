#include <stddef.h>
#include "imagepool.h"
#include "external/uthash_wrapper.h"
#include "gamelib/gameutil.h"

struct ImagePoolItem
{
	UT_hash_handle hh;

	char* filePath;
	size_t refCount;
	Texture2D texture;
	bool textureUpdated;
};

ImagePoolItem* PoolListHead = NULL;

static inline ImagePoolItem* FindItemByFilePath(const char* path)
{
	ImagePoolItem* item = NULL;

	HASH_FIND_STR(PoolListHead, path, item);

	return item;
}

static bool LocalLoadTexture(Texture2D* texture, const char* path)
{
	*texture = LoadTexture(path);

	if ( texture->id == 0 )
	{
		TraceLog(LOG_DEBUG, "IMAGEPOOL: Unable to create texture for %s", path);
		return false;
	}

	// Allow these to be customised in future?
	// We'd be getting more into material territory then.
	// For now, just let the user of the texture override
	// these manually later, if they want to. These values
	// are good enough defaults.
	SetTextureFilter(*texture, TEXTURE_FILTER_POINT);
	SetTextureWrap(*texture, TEXTURE_WRAP_CLAMP);

	return true;
}

static ImagePoolItem* CreateItem(const char* path)
{
	Image image = LoadImage(path);

	if ( !image.data )
	{
		return NULL;
	}

	Texture2D texture = { 0 };

	if ( !LocalLoadTexture(&texture, path) )
	{
		return NULL;
	}

	ImagePoolItem* item = (ImagePoolItem*)MemAlloc(sizeof(ImagePoolItem));

	item->filePath = DuplicateString(path);
	item->texture = texture;

	HASH_ADD_STR(PoolListHead, filePath, item);

	return item;
}

static void DestroyItem(ImagePoolItem* item)
{
	if ( !item )
	{
		return;
	}

	UnloadTexture(item->texture);
	MemFree(item->filePath);
	MemFree(item);
}

ImagePoolItem* ImagePool_AddRef(const char* path)
{
	if ( !path || !(*path) )
	{
		return NULL;
	}

	ImagePoolItem* item = FindItemByFilePath(path);

	if ( !item )
	{
		item = CreateItem(path);

		if ( !item )
		{
			return NULL;
		}
	}

	++item->refCount;
	return item;
}

void ImagePool_RemoveRef(ImagePoolItem* item)
{
	if ( !item )
	{
		return;
	}

	if ( item->refCount > 0 )
	{
		--item->refCount;
	}

	if ( item->refCount < 1 )
	{
		DestroyItem(item);
	}
}

Texture2D* ImagePool_GetTexture(ImagePoolItem* item)
{
	return item ? &item->texture : NULL;
}

const char* ImagePool_GetFilePath(ImagePoolItem* item)
{
	return item ? item->filePath : NULL;
}
