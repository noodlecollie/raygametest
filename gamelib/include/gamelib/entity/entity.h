#pragma once

#include "gamelib/external/raylibheaders.h"

struct EntityImpl;
struct PhysicsComponent;

typedef struct Entity
{
	Vector2 position;

	struct EntityImpl* impl;
} Entity;

struct PhysicsComponent* Entity_GetPhysicsComponent(Entity* ent);
struct PhysicsComponent* Entity_AddPhysicsComponent(Entity* ent);
void Entity_RemovePhysicsComponent(Entity* ent);
