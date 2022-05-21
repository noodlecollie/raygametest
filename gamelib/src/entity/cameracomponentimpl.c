#include "entity/cameracomponentimpl.h"

CameraComponentImpl* CameraComponentImpl_Create(struct Entity* ownerEntity)
{
	if ( !ownerEntity )
	{
		return NULL;
	}

	CameraComponentImpl* impl = (CameraComponentImpl*)MemAlloc(sizeof(CameraComponentImpl));

	impl->component.impl = impl;
	impl->ownerEntity = ownerEntity;
	impl->component.zoom = 1.0f;

	return impl;
}

void CameraComponentImpl_Destroy(CameraComponentImpl* impl)
{
	if ( !impl )
	{
		return;
	}

	MemFree(impl);
}

struct Entity* CameraComponent_GetOwnerEntity(const CameraComponent* component)
{
	return component ? component->impl->ownerEntity : NULL;
}
