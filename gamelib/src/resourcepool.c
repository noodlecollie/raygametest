#include "resourcepool.h"
#include "external/uthash_wrapper.h"
#include "gamelib/gameutil.h"
#include "descriptor/spritesheetdescriptor.h"

// Conveniences for this file, given we use meta.hh instead of just hh:

#define RPHASH_FIND_STR(head,findstr,out)                                        \
do {                                                                             \
    unsigned _uthash_hfstr_keylen = (unsigned)uthash_strlen(findstr);            \
    HASH_FIND(meta.hh, head, findstr, _uthash_hfstr_keylen, out);                \
} while (0)

#define RPHASH_ADD_STR(head,strfield,add)                                        \
do {                                                                             \
    unsigned _uthash_hastr_keylen = (unsigned)uthash_strlen((add)->strfield);    \
    HASH_ADD(meta.hh, head, strfield[0], _uthash_hastr_keylen, add);             \
} while (0)

typedef struct ResourceMeta
{
	UT_hash_handle hh;
	char* key;
	size_t refCount;
} ResourceMeta;

struct ResourcePoolTexture
{
	ResourceMeta meta;
	Texture2D texture;
};

typedef struct ResourcePool
{
	ResourcePoolTexture* textures;
} ResourcePool;

static ResourcePool Pool;

static inline void CleanUpMeta(ResourceMeta* meta)
{
	MemFree(meta->key);
}

static inline ResourcePoolTexture* FindTextureByPath(const char* path)
{
	ResourcePoolTexture* item = NULL;

	RPHASH_FIND_STR(Pool.textures, path, item);

	return item;
}

static bool LocalLoadTexture(Texture2D* texture, const char* path)
{
	*texture = LoadTexture(path);

	if ( texture->id == 0 )
	{
		TraceLog(LOG_DEBUG, "RESOURCEPOOL: Unable to create texture for %s", path);
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

static ResourcePoolTexture* CreateTextureItem(const char* path)
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

	ResourcePoolTexture* item = (ResourcePoolTexture*)MemAlloc(sizeof(ResourcePoolTexture));

	item->meta.key = DuplicateString(path);
	item->texture = texture;

	RPHASH_ADD_STR(Pool.textures, meta.key, item);

	return item;
}

static void DestroyTextureItem(ResourcePoolTexture* item)
{
	if ( !item )
	{
		return;
	}

	UnloadTexture(item->texture);
	CleanUpMeta(&item->meta);
	MemFree(item);
}

ResourcePoolTexture* ResourcePool_LoadTextureAndAddRef(const char* path)
{
	if ( !path || !(*path) )
	{
		return NULL;
	}

	ResourcePoolTexture* item = FindTextureByPath(path);

	if ( !item )
	{
		item = CreateTextureItem(path);

		if ( !item )
		{
			return NULL;
		}
	}

	return ResourcePool_AddTextureRef(item);
}

ResourcePoolTexture* ResourcePool_AddTextureRef(ResourcePoolTexture* item)
{
	if ( !item )
	{
		return NULL;
	}

	++item->meta.refCount;

	return item;
}

void ResourcePool_RemoveTextureRef(ResourcePoolTexture* item)
{
	if ( !item )
	{
		return;
	}

	if ( item->meta.refCount > 0 )
	{
		--item->meta.refCount;
	}

	if ( item->meta.refCount < 1 )
	{
		DestroyTextureItem(item);
	}
}

Texture2D* ResourcePool_GetTexture(ResourcePoolTexture* item)
{
	return item ? &item->texture : NULL;
}

const char* ResourcePool_GetTextureFilePath(ResourcePoolTexture* item)
{
	return item ? item->meta.key : NULL;
}
