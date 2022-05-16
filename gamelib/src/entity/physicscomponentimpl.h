#pragma once

#include "gamelib/entity/physicscomponent.h"

struct Entity;

typedef struct PhysicsComponentImpl
{
	struct Entity* ownerEntity;
	PhysicsComponent component;
} PhysicsComponentImpl;

PhysicsComponentImpl* PhysicsComponentImpl_Create(struct Entity* ownerEntity);
void PhysicsComponentImpl_Destroy(PhysicsComponentImpl* impl);
