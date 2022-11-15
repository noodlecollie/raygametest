#pragma once

#include <stdbool.h>
#include <stddef.h>

struct WorldImpl;
struct Entity;
struct CameraComponent;
struct EntityGroup;

typedef struct World
{
	float gravity;
	struct WorldImpl* impl;
} World;

World* World_Create(void);
void World_Destroy(World* world);

struct EntityGroup* World_GetDefaultEntityGroup(World* world);

struct Entity* World_CreateEntity(struct EntityGroup* group);
struct Entity* World_CreateEntityInDefaultGroup(World* world);
void World_DestroyEntity(struct Entity* ent);

struct Entity* World_GetEntityGroupHead(struct EntityGroup* group);
struct Entity* World_GetPreviousEntity(struct Entity* ent);
struct Entity* World_GetNextEntity(struct Entity* ent);
size_t World_GetEntityCount(const struct EntityGroup* group);

bool World_BeginLoadEntityGroup(World* world, const char* filePath);
bool World_IsLoadingEntityGroup(World* world);

void World_Update(World* world);

// BeginDrawing() should be called before this function is called.
void World_Render(World* world, struct CameraComponent* camera);
