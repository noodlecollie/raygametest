#pragma once

#include <stdbool.h>

struct Entity;

typedef struct LogicComponent
{
	struct Entity* ownerEntity;

	void (*onComponentRemoved)(struct LogicComponent* component);
	void (*onEntityDestroyed)(struct LogicComponent* component);
} LogicComponent;
