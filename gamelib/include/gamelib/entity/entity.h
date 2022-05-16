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
struct PhysicsComponent* Entity_AddPhysicsComponent(Entity* ent);
void Entity_RemovePhysicsComponent(Entity* ent);

struct TerrainComponent* Entity_GetTerrainComponent(Entity* ent);
struct TerrainComponent* Entity_AddTerrainComponent(Entity* ent);
void Entity_RemoveTerrainComponent(Entity* ent);

struct LogicComponent* Entity_GetLogicComponent(Entity* ent);
struct LogicComponent* Entity_AddLogicComponent(Entity* ent);
void Entity_RemoveLogicComponent(Entity* ent);
