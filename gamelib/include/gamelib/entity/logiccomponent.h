#pragma once

#include <stdbool.h>

struct Entity;
struct LogicComponentImpl;

typedef struct LogicComponent
{
	struct LogicComponentImpl* impl;

	void (*onComponentRemoved)(struct LogicComponent* component);
	void (*onEntityDestroyed)(struct Entity* thisEntity);
	void (*onPreThink)(struct Entity* thisEntity);
	void (*onPhysicsCollided)(struct Entity* thisEntity, struct Entity* otherEntity);
} LogicComponent;

struct Entity* LogicComponent_GetOwnerEntity(LogicComponent* component);
