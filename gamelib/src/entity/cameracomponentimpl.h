#pragma once

#include "gamelib/entity/cameracomponent.h"

struct Entity;

typedef struct CameraComponentImpl
{
	struct Entity* ownerEntity;
	CameraComponent component;
} CameraComponentImpl;

CameraComponentImpl* CameraComponentImpl_Create(struct Entity* ownerEntity);
void CameraComponentImpl_Destroy(CameraComponentImpl* impl);
