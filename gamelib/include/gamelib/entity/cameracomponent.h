#pragma once

#include "gamelib/external/raylibheaders.h"

struct Entity;
struct CameraComponentImpl;

typedef struct CameraComponent
{
	struct CameraComponentImpl* impl;

	float zoom;
} CameraComponent;

struct Entity* CameraComponent_GetOwnerEntity(const CameraComponent* component);
