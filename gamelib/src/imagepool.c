#include <stddef.h>
#include "imagepool.h"
#include "external/uthash_wrapper.h"
#include "gamelib/gameutil.h"

struct ImagePoolItem
{
	UT_hash_handle hh;

	char* filePath;
	size_t refCount;
	Image image;
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

static ImagePoolItem* CreateItem(const char* path)
{
	Image image = LoadImage(path);

	if ( !image.data )
	{
		return NULL;
	}

	ImagePoolItem* item = (ImagePoolItem*)MemAlloc(sizeof(ImagePoolItem));

	item->filePath = DuplicateString(path);
	item->image = image;

	HASH_ADD_STR(PoolListHead, filePath, item);

	return item;
}

static void DestroyItem(ImagePoolItem* item)
{
	if ( !item )
	{
		return;
	}

	UnloadImage(item->image);
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

Image* ImagePool_GetImage(ImagePoolItem* item)
{
	return item ? &item->image : NULL;
}

Texture2D* ImagePool_GetTexture(ImagePoolItem* item)
{
	return item ? &item->texture : NULL;
}

const char* ImagePool_GetFilePath(ImagePoolItem* item)
{
	return item ? item->filePath : NULL;
}

void ImagePool_FlagTextureNeedsUpdate(ImagePoolItem* item)
{
	if ( !item )
	{
		return;
	}

	item->textureUpdated = false;
}

bool ImagePool_EnsureTextureUpdated(ImagePoolItem* item)
{
	if ( !item )
	{
		return false;
	}

	if ( !item->textureUpdated && item->image.data )
	{
		if ( item->texture.id )
		{
			UnloadTexture(item->texture);
		}

		item->texture = LoadTextureFromImage(item->image);
		item->textureUpdated = item->texture.id != 0;

		if ( item->texture.id != 0 )
		{
			// Allow these to be customised in future?
			// We'd be getting more into material territory then.
			SetTextureFilter(item->texture, TEXTURE_FILTER_POINT);
			SetTextureWrap(item->texture, TEXTURE_WRAP_CLAMP);
		}
	}

	if ( !item->textureUpdated )
	{
		TraceLog(LOG_DEBUG, "IMAGEPOOL: Unable to update GPU texture for %s", item->filePath);
	}

	return item->textureUpdated;
}
