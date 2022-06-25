#include "rendering/debugrendercustom_impl.h"
#include "gamelib/debugging/debugging.h"
#include "gamelib/debugging/debugrendercustom.h"
#include "rendering/debugrendering.h"
#include "listmacros.h"

typedef enum
{
	ITEM_RECTANGLE
} DebugRenderItemType;

typedef struct DebugRenderItem
{
	struct DebugRenderItem* prev;
	struct DebugRenderItem* next;

	double expiryTime;
	DebugRenderItemType itemType;
	Vector2 position;
	Vector2 dimensions;
	Color colour;
} DebugRenderItem;

typedef struct DebugRenderList
{
	DebugRenderItem* head;
	DebugRenderItem* tail;
} DebugRenderList;

static DebugRenderList RenderList;
static size_t RefCount = 0;

static DebugRenderItem* CreateNewItem(float lifetime)
{
	DebugRenderItem* item = (DebugRenderItem*)MemAlloc(sizeof(DebugRenderItem));
	DBL_LL_ADD_TO_TAIL(item, prev, next, &RenderList, head, tail);

	item->expiryTime = GetTime() + (double)lifetime;

	return item;
}

static void RenderItem(DebugRenderItem* item)
{
	switch ( item->itemType )
	{
		case ITEM_RECTANGLE:
		{
			DebugRender_Rectangle((Rectangle){ item->position.x, item->position.y, item->dimensions.x, item->dimensions.y }, item->colour);
			break;
		};

		default:
		{
			break;
		}
	}
}

void DebugRenderCustom_Rectangle(Rectangle rect, Color colour, float lifetime)
{
	if ( !Debugging.debuggingEnabled )
	{
		return;
	}

	DebugRenderItem* item = CreateNewItem(lifetime);

	item->itemType = ITEM_RECTANGLE;
	item->position = (Vector2){ rect.x, rect.y };
	item->dimensions = (Vector2){ rect.width, rect.height };
	item->colour = colour;
}

void DebugRenderCustom_AddRef(void)
{
	if ( (RefCount + 1) < RefCount )
	{
		TraceLog(LOG_FATAL, "DEBUG RENDER CUSTOM: Ref count overflow!");
		return;
	}

	++RefCount;
}

void DebugRenderCustom_RemoveRef(void)
{
	if ( RefCount < 1 )
	{
		TraceLog(LOG_FATAL, "DEBUG RENDER CUSTOM: Ref count underflow!");
		return;
	}

	--RefCount;

	if ( RefCount < 1 )
	{
		DebugRenderCustom_ClearAll();
	}
}

void DebugRenderCustom_RenderAll(void)
{
	if ( !Debugging.debuggingEnabled || !Debugging.renderCustomDebugItems )
	{
		return;
	}

	DebugRenderItem* item = RenderList.head;
	const double now = GetTime();

	while ( item )
	{
		RenderItem(item);

		if ( item->expiryTime <= now )
		{
			DebugRenderItem* next = item->next;

			DBL_LL_REMOVE(item, prev, next, &RenderList, head, tail);
			MemFree(item);

			item = next;
		}
		else
		{
			item = item->next;
		}
	}
}

void DebugRenderCustom_ClearAll(void)
{
	DebugRenderItem* item = RenderList.head;

	while ( item )
	{
		DebugRenderItem* next = item->next;

		DBL_LL_REMOVE(item, prev, next, &RenderList, head, tail);
		MemFree(item);

		item = next;
	}
}
