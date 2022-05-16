#pragma once

#include <stdbool.h>

struct Entity;
struct LogicComponent;
struct LogicComponentImpl;

typedef struct LogicComponentCallbacks
{
	void (*onComponentCleanup)(struct LogicComponent* component);
	void (*onEntityDestroyed)(struct Entity* thisEntity);
	void (*onPreThink)(struct Entity* thisEntity);
	void (*onPhysicsCollided)(struct Entity* thisEntity, struct Entity* otherEntity);
} LogicComponentCallbacks;

typedef struct LogicComponent
{
	struct LogicComponentImpl* impl;

	LogicComponentCallbacks callbacks;
	void* userData;
} LogicComponent;

struct Entity* LogicComponent_GetOwnerEntity(LogicComponent* component);
void LogicComponent_PerformCleanup(LogicComponent* component);
