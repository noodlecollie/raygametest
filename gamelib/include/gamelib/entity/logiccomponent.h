#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "gamelib/entity/componenttypes.h"

struct Entity;
struct LogicComponent;
struct LogicComponentImpl;

typedef struct OnPhysicsCollidedArgs
{
	struct Entity* initiator;
	ComponentType initiatorComponentType;

	struct Entity* recipient;
	ComponentType recipientComponentType;

	size_t simIteration;
} OnPhysicsCollidedArgs;

typedef struct LogicComponentCallbacks
{
	void (*onComponentCleanup)(struct LogicComponent* component);
	void (*onEntityDestroyed)(struct LogicComponent* component);
	void (*onPreThink)(struct LogicComponent* component);
	void (*onPostThink)(struct LogicComponent* component);
	void (*onPhysicsCollided)(struct LogicComponent* component, const OnPhysicsCollidedArgs* args);
} LogicComponentCallbacks;

typedef struct LogicComponent
{
	struct LogicComponentImpl* impl;

	LogicComponentCallbacks callbacks;
	uint32_t userDataType;
	void* userData;
} LogicComponent;

struct Entity* LogicComponent_GetOwnerEntity(const LogicComponent* component);
void LogicComponent_PerformCleanup(LogicComponent* component);
