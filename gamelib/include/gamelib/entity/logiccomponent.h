#pragma once

#include <stdbool.h>

struct Entity;
struct PhysicsComponent;

typedef struct LogicComponent
{
	struct Entity* ownerEntity;

	void (*onComponentRemoved)(struct LogicComponent* component);
	void (*onEntityDestroyed)(struct Entity* thisEntity);
	void (*onPreThink)(struct Entity* thisEntity);
	void (*onPhysicsCollided)(struct Entity* thisEntity, struct Entity* otherEntity);
} LogicComponent;
