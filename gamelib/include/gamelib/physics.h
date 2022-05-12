#pragma once

#define PHYSICS_CONTACT_ADJUST_DIST 0.01f

#include "gamelib/world.h"
#include "entity/physicscomponent.h"

void Physics_Simulate(World* world, PhysicsComponent* component);
