#pragma once

#define WORLD_MAX_ENTITIES 2048

struct WorldImpl;
struct Entity;

typedef struct World
{
	struct WorldImpl* impl;

	float gravity;
} World;

World* World_Create(void);
void World_Destroy(World* world);

struct Entity* World_CreateEntity(World* world);
void World_DestroyEntity(struct Entity* ent);
struct Entity* World_GetEntityListHead(World* world);
struct Entity* World_GetPreviousEntity(struct Entity* ent);
struct Entity* World_GetNextEntity(struct Entity* ent);
size_t World_GetEntityCount(const World* world);

void World_Think(World* world);
void World_Render(World* world);
