#pragma once

#include <stdint.h>
#include "gamelib/entity/physicscomponent.h"

typedef struct Entity* Entity;

Entity Entity_Create(void);
void Entity_Destroy(Entity ent);

PhysicsComponent* Entity_PhysicsComponent(Entity ent);
void Entity_RemovePhysicsComponent(Entity ent);
