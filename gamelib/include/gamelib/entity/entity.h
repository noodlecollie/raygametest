#pragma once

#include "gamelib/external/raylibheaders.h"

struct EntityImpl;
struct PhysicsComponent;
struct TerrainComponent;
struct LogicComponent;

typedef struct Entity
{
	Vector2 position;
	struct EntityImpl* impl;
} Entity;

struct PhysicsComponent* Entity_GetPhysicsComponent(Entity* ent);
struct PhysicsComponent* Entity_CreatePhysicsComponent(Entity* ent);
void Entity_DestroyPhysicsComponent(Entity* ent);

struct TerrainComponent* Entity_GetTerrainComponent(Entity* ent);
struct TerrainComponent* Entity_CreateTerrainComponent(Entity* ent);
void Entity_DestroyTerrainComponent(Entity* ent);

struct LogicComponent* Entity_GetLogicComponent(Entity* ent);
struct LogicComponent* Entity_CreateLogicComponent(Entity* ent);
void Entity_DestroyLogicComponent(Entity* ent);
