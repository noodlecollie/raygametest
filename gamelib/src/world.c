#include <math.h>
#include <float.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/world.h"
#include "gamelib/physics.h"
#include "gamelib/entity/entity.h"
#include "entity/entityimpl.h"
#include "entity/cameracomponentimpl.h"
#include "gamelib/entity/logiccomponent.h"
#include "gamelib/entity/spritecomponent.h"
#include "gamelib/entity/terraincomponent.h"
#include "listmacros.h"
#include "rendering/renderutils.h"
#include "rendering/spriterenderer.h"
#include "gamelib/debugging/debugging.h"
#include "rendering/debugrendering.h"
#include "rendering/debugrendercustom_impl.h"

typedef struct WorldImpl
{
	size_t entityCount;
	EntityImpl* entitiesHead;
	EntityImpl* entitiesTail;

	CameraComponent* activeCamera;
} WorldImpl;

static inline void IncreaseSubsystemRefCount()
{
	SpriteRenderer_AddRef();
	DebugRenderCustom_AddRef();
}

static inline void DecreaseSubsystemRefCount()
{
	DebugRenderCustom_RemoveRef();
	SpriteRenderer_RemoveRef();
}

static void DestroyAllEntities(WorldImpl* impl)
{
	EntityImpl* slot = impl->entitiesHead;

	impl->entitiesHead = NULL;
	impl->entitiesTail = NULL;
	impl->entityCount = 0;

	while ( slot )
	{
		EntityImpl* next = slot->next;
		EntityImpl_Destroy(slot);
		slot = next;
	}
}

static void RenderDebug(World* world, Camera3D camera)
{
	for ( Entity* ent = World_GetEntityListHead(world); ent; ent = World_GetNextEntity(ent) )
	{
		DebugRender_Entity(ent->impl, camera);
	}

	if ( Debugging.renderCustomDebugItems )
	{
		DebugRenderCustom_RenderAll();
	}
}

World* World_Create(void)
{
	IncreaseSubsystemRefCount();

	World* world = (World*)MemAlloc(sizeof(World));
	world->impl = (WorldImpl*)MemAlloc(sizeof(WorldImpl));

	world->gravity = 800.0f;

	return world;
}

void World_Destroy(World* world)
{
	if ( !world )
	{
		return;
	}

	DestroyAllEntities(world->impl);
	MemFree(world->impl);
	MemFree(world);

	DecreaseSubsystemRefCount();
}

struct Entity* World_CreateEntity(World* world)
{
	if ( !world )
	{
		return NULL;
	}

	WorldImpl* impl = world->impl;

	if ( impl->entityCount >= WORLD_MAX_ENTITIES )
	{
		return NULL;
	}

	EntityImpl* slot = (EntityImpl*)MemAlloc(sizeof(EntityImpl));

	slot->ownerWorld = world;
	slot->entity.impl = slot;

	DBL_LL_ADD_TO_TAIL(slot, prev, next, impl, entitiesHead, entitiesTail);

	++impl->entityCount;

	return &slot->entity;
}

void World_DestroyEntity(struct Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	EntityImpl* slot = ent->impl;

	if ( slot->prev )
	{
		slot->prev->next = slot->next;
	}
	else if ( slot->ownerWorld )
	{
		slot->ownerWorld->impl->entitiesHead = slot->next;
	}

	if ( slot->next )
	{
		slot->next->prev = slot->prev;
	}
	else if ( slot->ownerWorld )
	{
		slot->ownerWorld->impl->entitiesTail = slot->prev;
	}

	if ( slot->ownerWorld && slot->ownerWorld->impl->entityCount > 0 )
	{
		--slot->ownerWorld->impl->entityCount;
	}

	EntityImpl_Destroy(slot);
}

struct Entity* World_GetEntityListHead(World* world)
{
	return (world && world->impl->entitiesHead) ? &world->impl->entitiesHead->entity : NULL;
}

struct Entity* World_GetPreviousEntity(struct Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	return (ent && ent->impl->prev) ? &ent->impl->prev->entity : NULL;
}

struct Entity* World_GetNextEntity(struct Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	return (ent && ent->impl->next) ? &ent->impl->next->entity : NULL;
}

size_t World_GetEntityCount(const World* world)
{
	return world ? world->impl->entityCount : 0;
}

void World_Update(World* world)
{
	if ( !world )
	{
		return;
	}

	for ( Entity* ent = World_GetEntityListHead(world); ent; ent = World_GetNextEntity(ent) )
	{
		for ( LogicComponent* logic = Entity_GetLogicComponentListHead(ent); logic; logic = Entity_GetNextLogicComponent(logic) )
		{
			if ( logic->callbacks.onPreThink )
			{
				logic->callbacks.onPreThink(logic);
			}
		}

		if ( Entity_GetSpriteComponent(ent) )
		{
			SpriteComponentImpl_Update(ent->impl->spriteImpl);
		}

		if ( ent->impl->physicsImpl )
		{
			Physics_SimulateObjectInWorld(world, &ent->impl->physicsImpl->component);
		}

		for ( LogicComponent* logic = Entity_GetLogicComponentListHead(ent); logic; logic = Entity_GetNextLogicComponent(logic) )
		{
			if ( logic->callbacks.onPostThink )
			{
				logic->callbacks.onPostThink(logic);
			}
		}
	}
}

void World_SetActiveCamera(World* world, struct CameraComponent* camera)
{
	if ( !world )
	{
		return;
	}

	world->impl->activeCamera = camera;
}

/*
	To perform instanced rendering we need to set up a shader that supports it.
	The following is based off the default shader:

		static const char VS_CODE[] =
			"#version 330                       \n"
			"in vec3 vertexPosition;            \n"
			"in vec2 vertexTexCoord;            \n"
			"in vec4 vertexColor;               \n"
			"in mat4 instanceTransform;         \n"
			"out vec2 fragTexCoord;             \n"
			"out vec4 fragColor;                \n"
			"uniform mat4 mvp;                  \n"
			"void main()                        \n"
			"{                                  \n"
			"    mat4 mvpi = mvp*instanceTransform;\n"
			"    fragTexCoord = vertexTexCoord; \n"
			"    fragColor = vertexColor;       \n"
			"    gl_Position = mvpi*vec4(vertexPosition, 1.0); \n"
			"}                                  \n"
		;

		static const char FS_CODE[] =
			"#version 330       \n"
			"in vec2 fragTexCoord;              \n"
			"in vec4 fragColor;                 \n"
			"out vec4 finalColor;               \n"
			"uniform sampler2D texture0;        \n"
			"uniform vec4 colDiffuse;           \n"
			"void main()                        \n"
			"{                                  \n"
			"    vec4 texelColor = texture(texture0, fragTexCoord);   \n"
			"    finalColor = texelColor*colDiffuse*fragColor;        \n"
			"}                                  \n"
		;

		Shader shader = LoadShaderFromMemory(VS_CODE, FS_CODE);
		shader.locs[SHADER_LOC_VERTEX_POSITION] = GetShaderLocation(shader, "vertexPosition");
		shader.locs[SHADER_LOC_VERTEX_TEXCOORD01] = GetShaderLocation(shader, "vertexTexCoord");
		shader.locs[SHADER_LOC_VERTEX_COLOR] = GetShaderLocation(shader, "vertexColor");
		shader.locs[SHADER_LOC_MATRIX_MVP]  = GetShaderLocation(shader, "mvp");
		shader.locs[SHADER_LOC_COLOR_DIFFUSE] = GetShaderLocation(shader, "colDiffuse");
		shader.locs[SHADER_LOC_MAP_DIFFUSE] = GetShaderLocation(shader, "texture0");
		shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader, "instanceTransform");

		materialToUse.shader = shader;

		DrawMeshInstanced(mesh, materialToUse, transforms, ARRAY_SIZE(transforms));

	This demonstrates the main idea. However, storing loads of matrices for all the blocks in
	the terrain will probably be really inefficient on memory, so we could tweak this shader
	to optimise. We could provide the bounds for the area that we are drawing, and provide
	texture uniforms for other pieces of information.
 */
void World_Render(World* world)
{
	if ( !world )
	{
		return;
	}

	if ( !world->impl->activeCamera )
	{
		TraceLog(LOG_WARNING, "WORLD: Render call without active camera set!");
		return;
	}

	CameraComponentImpl* camImpl = world->impl->activeCamera->impl;

	if ( fabsf(camImpl->component.zoom) < FLT_EPSILON )
	{
		TraceLog(LOG_WARNING, "WORLD: Render camera had zoom of zero!");
		return;
	}

	Camera3D camera = { 0 };

	camera.projection = CAMERA_ORTHOGRAPHIC;
	camera.target = (Vector3){ camImpl->ownerEntity->position.x, camImpl->ownerEntity->position.y, CAMERA_FAR_DEPTH };

	camera.position = camera.target;
	camera.position.z = CAMERA_NEAR_DEPTH;

	camera.up = (Vector3){ 0.0f, -1.0f, 0.0f };
	camera.fovy = (float)GetScreenHeight() / camImpl->component.zoom;

	BeginMode3D(camera);

	for ( Entity* ent = World_GetEntityListHead(world); ent; ent = World_GetNextEntity(ent) )
	{
		EntityImpl_Render(ent->impl, camera);
	}

	if ( Debugging.debuggingEnabled )
	{
		RenderDebug(world, camera);
	}

	EndMode3D();
}
