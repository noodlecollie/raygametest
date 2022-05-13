#pragma once

#define PHYSICS_CONTACT_ADJUST_DIST 0.01f

#include "gamelib/oldworld.h"
#include "gamelib/oldentity/oldentity.h"

void Physics_Simulate(OldWorld* world, OldEntity* entity);
