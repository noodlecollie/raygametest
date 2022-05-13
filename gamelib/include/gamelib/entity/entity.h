#pragma once

#include "gamelib/external/raylibheaders.h"

struct EntityImpl;
struct PhysicsComponent;

typedef struct Entity
{
	Vector2 position;

	struct EntityImpl* impl;
} Entity;

struct PhysicsComponent* Entity_GetPhysicsComponent(Entity* entity);
struct PhysicsComponent* Entity_AddPhysicsComponent(Entity* entity);
void Entity_RemovePhysicsComponent(Entity* entity);
