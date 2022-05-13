#pragma once

#define WORLD_MAX_ENTITIES 2048

typedef struct World World;

World* World_Create(void);
void World_Destroy(World* world);
