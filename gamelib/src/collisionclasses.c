#include "gamelib/collisionclasses.h"
#include "gamelib/gameutil.h"

typedef struct
{
	const char* str;
	CollisionClass collisionClass;
} CollisionClassMapping;

static const CollisionClassMapping COLLISION_CLASSES[] =
{
	{ "detail", COLLISIONCLASS_DETAIL },
	{ "terrain", COLLISIONCLASS_TERRAIN },
	{ "item", COLLISIONCLASS_ITEM },
	{ "trigger", COLLISIONCLASS_TRIGGER },
};

bool CollisionClass_FromString(const char* str, CollisionClass* out)
{
	if ( !str || !out )
	{
		return false;
	}

	for ( size_t index = 0; index < ARRAY_SIZE(COLLISION_CLASSES); ++index )
	{
		if ( TextIsEqual(str, COLLISION_CLASSES[index].str) )
		{
			*out = COLLISION_CLASSES[index].collisionClass;
			return true;
		}
	}

	return false;
}
