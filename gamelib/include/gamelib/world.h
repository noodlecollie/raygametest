#pragma once

struct WorldImpl;
struct Entity;
struct CameraComponent;

typedef struct EntityGroup EntityGroup;

typedef struct World
{
	float gravity;
	struct WorldImpl* impl;
} World;

World* World_Create(void);
void World_Destroy(World* world);

EntityGroup* World_GetDefaultEntityGroup(World* world);

struct Entity* World_CreateEntity(EntityGroup* group);
struct Entity* World_CreateEntityInDefaultGroup(World* world);
void World_DestroyEntity(struct Entity* ent);

struct Entity* World_GetEntityGroupHead(EntityGroup* group);
struct Entity* World_GetPreviousEntity(struct Entity* ent);
struct Entity* World_GetNextEntity(struct Entity* ent);
size_t World_GetEntityCount(const EntityGroup* group);

void World_Update(World* world);

// BeginDrawing() should be called before this function is called.
void World_Render(World* world, struct CameraComponent* camera);
