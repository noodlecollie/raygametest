#pragma once

#define PHYSICS_CONTACT_ADJUST_DIST 0.01f

#include "gamelib/oldworld.h"
#include "gamelib/oldentity/oldentity.h"

struct World;
struct PhysicsComponent;

void Physics_SimulateOld(OldWorld* world, OldEntity* entity);

void Physics_SimulateInWorld(struct World* world, struct PhysicsComponent* physComp);
