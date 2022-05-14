#pragma once

#include "gamelib/external/raylibheaders.h"

struct EntityImpl;
struct PhysicsComponent;
struct TerrainComponent;

typedef struct Entity
{
	Vector2 position;
	struct EntityImpl* impl;
} Entity;

struct PhysicsComponent* Entity_GetPhysicsComponent(Entity* ent);
struct PhysicsComponent* Entity_AddPhysicsComponent(Entity* ent);
void Entity_RemovePhysicsComponent(Entity* ent);

struct TerrainComponent* Entity_GetTerraimComponent(Entity* ent);
struct TerrainComponent* Entity_AddTerrainComponent(Entity* ent);
void Entity_RemoveTerrainComponent(Entity* ent);
