#pragma once

#include <stdint.h>
#include "raylib.h"
#include "gamelib/oldentity/oldphysicscomponent.h"
#include "gamelib/gametypes.h"
#include "gamelib/oldentity/oldcomponenttypes.h"

struct OldEntityComponentList;

typedef struct OldEntity
{
	EntityIndex index;
	Vector2 position;
	struct OldEntityComponentList* components;
} OldEntity;

OldEntity* OldEntity_Create(void);
void OldEntity_Destroy(OldEntity* ent);

bool OldEntity_HasComponent(const OldEntity* ent, OldComponentType componentType);

OldPhysicsComponent* OldEntity_PhysicsComponent(OldEntity* ent);
OldPhysicsComponent* OldEntity_AddPhysicsComponent(OldEntity* ent);
void OldEntity_RemovePhysicsComponent(OldEntity* ent);
