#include <stddef.h>
#include "imagepool.h"
#include "external/uthash/uthash.h"
#include "gamelib/gameutil.h"

struct ImagePoolItem
{
	UT_hash_handle hh;

	char* filePath;
	size_t refCount;
	Image image;
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

	if ( !item && !(item = CreateItem(path)) )
	{
		return NULL;
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
