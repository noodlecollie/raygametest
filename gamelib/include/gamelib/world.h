#pragma once

#define WORLD_MAX_ENTITIES 2048

typedef struct World World;
struct Entity;

World* World_Create(void);
void World_Destroy(World* world);

struct Entity* World_CreateEntity(World* world);
void World_DestroyEntity(struct Entity* ent);
