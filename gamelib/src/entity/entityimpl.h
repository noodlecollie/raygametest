#pragma once

#include "gamelib/entity/entity.h"
#include "entity/physicscomponentimpl.h"
#include "entity/terraincomponentimpl.h"
#include "entity/logiccomponentimpl.h"
#include "entity/spritecomponentimpl.h"
#include "entity/cameracomponentimpl.h"

struct World;

typedef struct EntityImpl
{
	struct World* ownerWorld;

	Entity entity;
	struct EntityImpl* prev;
	struct EntityImpl* next;

	PhysicsComponentImpl* physicsImpl;
	TerrainComponentImpl* terrainImpl;
	SpriteComponentImpl* spriteImpl;
	CameraComponentImpl* cameraImpl;

	LogicComponentImpl* logicImplHead;
	LogicComponentImpl* logicImplTail;
} EntityImpl;

void EntityImpl_Destroy(EntityImpl* impl);
void EntityImpl_Render(EntityImpl* impl);
