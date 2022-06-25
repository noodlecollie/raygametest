#include "rendering/debugrendering.h"
#include "gamelib/debugging.h"
#include "rendering/renderutils.h"
#include "gamelib/gameutil.h"

static void DrawPhysicsHull(PhysicsComponentImpl* impl)
{
	Rectangle hull = PhysicsComponent_GetWorldCollisionHull(&impl->component);
	DebugRender_Rectangle(hull, DEBUG_COLOUR_PHYSICS);
}

void DebugRender_Entity(EntityImpl* impl, Camera3D camera)
{
	if ( !impl || !Debugging.debuggingEnabled )
	{
		return;
	}

	if ( Debugging.renderEntityLocations )
	{
		DrawDebugScreenSpaceCross(camera, impl->entity.position, 0.02f, DEBUG_COLOUR_ENTITY);
	}

	if ( impl->physicsImpl && Debugging.renderPhysicsHulls )
	{
		DrawPhysicsHull(impl->physicsImpl);
	}
}

void DebugRender_Rectangle(Rectangle rect, Color colour)
{
	Vector2 min = RectangleMin(rect);
	Vector2 max = RectangleMax(rect);

	DrawLine3D(
		(Vector3){ min.x, min.y, DEBUG_OVERLAY_DEPTH },
		(Vector3){ max.x, min.y, DEBUG_OVERLAY_DEPTH },
		colour
	);

	DrawLine3D(
		(Vector3){ max.x, min.y, DEBUG_OVERLAY_DEPTH },
		(Vector3){ max.x, max.y, DEBUG_OVERLAY_DEPTH },
		colour
	);

	DrawLine3D(
		(Vector3){ max.x, max.y, DEBUG_OVERLAY_DEPTH },
		(Vector3){ min.x, max.y, DEBUG_OVERLAY_DEPTH },
		colour
	);

	DrawLine3D(
		(Vector3){ min.x, max.y, DEBUG_OVERLAY_DEPTH },
		(Vector3){ min.x, min.y, DEBUG_OVERLAY_DEPTH },
		colour
	);
}
