#pragma once

#define PHYSICS_CONTACT_ADJUST_DIST 0.01f

#include "gamelib/world.h"
#include "entity/entity.h"

void Physics_Simulate(World* world, Entity* entity);
