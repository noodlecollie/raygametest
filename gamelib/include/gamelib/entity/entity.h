#pragma once

#include <stdint.h>
#include "raylib.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/gametypes.h"
#include "gamelib/entity/componenttypes.h"

struct EntityComponentList;

typedef struct Entity
{
	EntityIndex index;
	Vector2 position;
	struct EntityComponentList* components;
} Entity;

Entity* Entity_Create(void);
void Entity_Destroy(Entity* ent);

bool Entity_HasComponent(const Entity* ent, ComponentType componentType);

PhysicsComponent* Entity_PhysicsComponent(Entity* ent);
PhysicsComponent* Entity_AddPhysicsComponent(Entity* ent);
void Entity_RemovePhysicsComponent(Entity* ent);
