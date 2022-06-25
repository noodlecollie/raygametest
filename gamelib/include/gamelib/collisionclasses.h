#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "gamelib/gametypes.h"

typedef uint32_t CollisionClass;
typedef Mask32 CollisionMask;

#define COLLISIONCLASS_DETAIL 0
#define COLLISIONCLASS_TERRAIN 1
#define COLLISIONCLASS_ITEM 2
#define COLLISIONCLASS_TRIGGER 3

#define COLLISIONMASK_DETAIL (1 << COLLISIONCLASS_DETAIL)
#define COLLISIONMASK_TERRAIN (1 << COLLISIONCLASS_TERRAIN)
#define COLLISIONMASK_ITEM (1 << COLLISIONCLASS_ITEM)
#define COLLISIONMASK_TRIGGER (1 << COLLISIONCLASS_TRIGGER)
#define COLLISIONMASK_NONE 0
#define COLLISIONMASK_ALL ((Mask32)~0)
#define COLLISIONCLASS_MAX_CLASSES MASK32_BITS

bool CollisionClass_FromString(const char* str, CollisionClass* out);
