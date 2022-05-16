#pragma once

#include "gamelib/entity/logiccomponent.h"

struct Entity;

typedef struct LogicComponentImpl
{
	struct Entity* ownerEntity;
	LogicComponent component;
} LogicComponentImpl;

LogicComponentImpl* LogicComponentImpl_Create(struct Entity* ownerEntity);
void LogicComponentImpl_Destroy(LogicComponentImpl* impl);
