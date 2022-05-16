#pragma once

#include <stdbool.h>

struct Entity;
struct LogicComponent;
struct LogicComponentImpl;

typedef struct LogicComponentCallbacks
{
	void (*onComponentCleanup)(struct LogicComponent* component);
	void (*onEntityDestroyed)(struct LogicComponent* component);
	void (*onPreThink)(struct LogicComponent* component);
	void (*onPostThink)(struct LogicComponent* component);
	void (*onPhysicsCollided)(struct LogicComponent* component, struct Entity* otherEntity);
} LogicComponentCallbacks;

typedef struct LogicComponent
{
	struct LogicComponentImpl* impl;

	LogicComponentCallbacks callbacks;
	void* userData;
} LogicComponent;

struct Entity* LogicComponent_GetOwnerEntity(LogicComponent* component);
void LogicComponent_PerformCleanup(LogicComponent* component);
