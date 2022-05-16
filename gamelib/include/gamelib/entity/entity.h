#pragma once

#include "gamelib/external/raylibheaders.h"

struct EntityImpl;
struct PhysicsComponent;
struct TerrainComponent;
struct LogicComponent;
struct World;

typedef struct Entity
{
	Vector2 position;
	struct EntityImpl* impl;
} Entity;

struct World* Entity_GetWorld(Entity* ent);

struct PhysicsComponent* Entity_GetPhysicsComponent(Entity* ent);
struct PhysicsComponent* Entity_CreatePhysicsComponent(Entity* ent);
void Entity_DestroyPhysicsComponent(Entity* ent);

struct TerrainComponent* Entity_GetTerrainComponent(Entity* ent);
struct TerrainComponent* Entity_CreateTerrainComponent(Entity* ent);
void Entity_DestroyTerrainComponent(Entity* ent);

struct LogicComponent* Entity_GetLogicComponentListHead(Entity* ent);
struct LogicComponent* Entity_AddLogicComponent(Entity* ent);
void Entity_RemoveLogicComponent(struct LogicComponent* component);
void Entity_RemoveAllLogicComponents(Entity* ent);
struct LogicComponent* Entity_GetPreviousLogicComponent(struct LogicComponent* component);
struct LogicComponent* Entity_GetNextLogicComponent(struct LogicComponent* component);
