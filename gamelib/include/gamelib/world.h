#pragma once

#define WORLD_MAX_ENTITIES 2048

struct WorldImpl;
struct Entity;

typedef struct World
{
	float gravity;
	struct WorldImpl* impl;
} World;

World* World_Create(void);
void World_Destroy(World* world);

struct Entity* World_CreateEntity(World* world);
void World_DestroyEntity(struct Entity* ent);
struct Entity* World_GetEntityListHead(World* world);
struct Entity* World_GetPreviousEntity(struct Entity* ent);
struct Entity* World_GetNextEntity(struct Entity* ent);

void World_Think(World* world);
