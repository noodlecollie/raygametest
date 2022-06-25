#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "gamelib/trace.h"
#include "gamelib/gametypes.h"
#include "gamelib/collisionclasses.h"

struct World;
struct PhysicsComponent;
struct Entity;

#define PHYSICS_CONTACT_ADJUST_DIST 0.01f

void Physics_SimulateObjectInWorld(struct World* world, struct PhysicsComponent* physComp);

TraceResult Physics_TraceHullInWorld(
	struct World* world,
	Rectangle hull,
	Vector2 delta,
	CollisionMask collisionMask,
	struct Entity* hullOwner
);
