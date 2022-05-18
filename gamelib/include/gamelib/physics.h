#pragma once

#define PHYSICS_CONTACT_ADJUST_DIST 0.01f

#include "gamelib/trace.h"

struct World;
struct PhysicsComponent;
struct Entity;

void Physics_SimulateObjectInWorld(struct World* world, struct PhysicsComponent* physComp);

TraceResult Physics_TraceHullInWorld(
	struct World* world,
	Rectangle hull,
	Vector2 delta,
	Mask32 collisionMask,
	struct Entity* hullOwner
);
