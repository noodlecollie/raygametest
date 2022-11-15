#include <math.h>
#include <float.h>
#include <pthread.h>
#include "entity/entitygroup.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/world.h"
#include "gamelib/physics.h"
#include "gamelib/entity/entity.h"
#include "raylib.h"
#include "worldimpl.h"
#include "entity/entityimpl.h"
#include "entity/cameracomponentimpl.h"
#include "gamelib/entity/logiccomponent.h"
#include "gamelib/entity/spritecomponent.h"
#include "gamelib/entity/terraincomponent.h"
#include "gamelib/debugging/debugging.h"
#include "gamelib/stringutil.h"
#include "listmacros.h"
#include "rendering/renderutils.h"
#include "rendering/spriterenderer.h"
#include "rendering/debugrendering.h"
#include "rendering/debugrendercustom_impl.h"
#include "asyncload/asyncload.h"
#include "threading.h"

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

static void RenderDebug(World* world, Camera3D camera)
{
	for ( Entity* ent = World_GetEntityGroupHead(world->impl->defaultEntityGroup); ent; ent = World_GetNextEntity(ent) )
	{
		DebugRender_Entity(ent->impl, camera);
	}

	if ( Debugging.renderCustomDebugItems )
	{
		DebugRenderCustom_RenderAll();
	}
}

static inline bool AsyncThreadIsRunning(WorldImpl* impl)
{
	return impl->asyncLoadThreadArgs != NULL;
}

static inline bool AsyncLoadThreadIsFinished(WorldImpl* impl)
{
	return THREADING_GET_SEMAPHORE_VALUE(&impl->asyncLoadSemaphore) == ASYNCLOAD_SEM_READY;
}

static void CreateAsyncLoadThreadArgs(WorldImpl* impl, const char* filePath)
{
	if ( impl->asyncLoadThreadArgs )
	{
		return;
	}

	impl->asyncLoadThreadArgs = AsyncLoadArgs_Create(&impl->asyncLoadSemaphore, filePath);
}

static void DestroyAsyncLoadThreadArgs(WorldImpl* impl)
{
	AsyncLoadArgs_Destroy(impl->asyncLoadThreadArgs);
	impl->asyncLoadThreadArgs = NULL;
}

static void JoinAsyncLoadThread(WorldImpl* impl)
{
	if ( !AsyncThreadIsRunning(impl) )
	{
		return;
	}

	TraceLog(LOG_DEBUG, "WORLD: Joining entity async load thread.");

	// TODO: Do we want a timeout on this?
	// If it doesn't join in time, do we want to throw a fatal error?
	THREADING_JOIN(impl->asyncLoadThread, NULL);

	if ( impl->asyncLoadThreadArgs->loadedEntityGroup )
	{
		// A group was loaded, so take ownership of it.

		TraceLog(
			LOG_INFO,
			"WORLD: [%s] Entity group of %zu entities was loaded asynchronously.",
			impl->asyncLoadThreadArgs->filePath,
			impl->asyncLoadThreadArgs->loadedEntityGroup->count
		);

		impl->asyncLoadedEntityGroup = impl->asyncLoadThreadArgs->loadedEntityGroup;
		impl->asyncLoadThreadArgs->loadedEntityGroup = NULL;
	}
	else
	{
		TraceLog(LOG_WARNING, "WORLD: [%s] Async entity load failed to load any entity group.", impl->asyncLoadThreadArgs->filePath);
	}

	DestroyAsyncLoadThreadArgs(impl);
}

static void DestroyUnintegratedEntityGroup(WorldImpl* impl)
{
	const int semaphoreValue = THREADING_GET_SEMAPHORE_VALUE(&impl->asyncLoadSemaphore);

	// Semaphore should never be busy when we do this.
	if ( semaphoreValue != ASYNCLOAD_SEM_IDLE && semaphoreValue != ASYNCLOAD_SEM_READY )
	{
		TraceLog(LOG_FATAL, "WORLD: Unexpected async load semaphore value of %d when destroying unintegrated entity group.", semaphoreValue);
		return;
	}

	if ( impl->asyncLoadedEntityGroup )
	{
		TraceLog(LOG_INFO, "WORLD: Entity group of %zu entities being discarded after async load.", impl->asyncLoadedEntityGroup->count);

		EntityGroup_Destroy(impl->asyncLoadedEntityGroup);
		impl->asyncLoadedEntityGroup = NULL;
	}

	if ( semaphoreValue != ASYNCLOAD_SEM_IDLE )
	{
		// This function was called before we'd finalised the entity group.
		// Decrement the semaphore back to idle, just in case it
		// would screw anything up otherwise.
		THREADING_DECREMENT_SEMAPHORE(&impl->asyncLoadSemaphore);
	}

	THREADING_ASSERT_SEMAPHORE_HAS_VALUE(&impl->asyncLoadSemaphore, ASYNCLOAD_SEM_IDLE);
}

static void IntegrateEntityGroup(WorldImpl* impl)
{
	JoinAsyncLoadThread(impl);

	if ( impl->asyncLoadedEntityGroup )
	{
		TraceLog(LOG_INFO, "WORLD: Integrating new group of %zu entities.", impl->asyncLoadedEntityGroup->count);

		// TODO: Do this properly
		EntityGroup_Destroy(impl->asyncLoadedEntityGroup);
		impl->asyncLoadedEntityGroup = NULL;
	}

	THREADING_DECREMENT_SEMAPHORE(&impl->asyncLoadSemaphore);
	THREADING_ASSERT_SEMAPHORE_HAS_VALUE(&impl->asyncLoadSemaphore, ASYNCLOAD_SEM_IDLE);
}

World* World_Create(void)
{
	IncreaseSubsystemRefCount();

	WorldImpl* impl = (WorldImpl*)MemAlloc(sizeof(WorldImpl));

	impl->world.impl = impl;
	impl->defaultEntityGroup = EntityGroup_Create(impl);

	THREADING_INIT_SEMAPHORE(&impl->asyncLoadSemaphore, 0, ASYNCLOAD_SEM_IDLE);

	impl->world.gravity = 800.0f;

	return &impl->world;
}

void World_Destroy(World* world)
{
	if ( !world )
	{
		return;
	}

	WorldImpl* impl = world->impl;

	JoinAsyncLoadThread(impl);
	DestroyUnintegratedEntityGroup(impl);

	EntityGroup_Destroy(impl->defaultEntityGroup);
	THREADING_DESTROY_SEMAPHORE(&impl->asyncLoadSemaphore);

	MemFree(impl);

	DecreaseSubsystemRefCount();
}

struct EntityGroup* World_GetDefaultEntityGroup(World* world)
{
	return world ? world->impl->defaultEntityGroup : NULL;
}

struct Entity* World_CreateEntity(EntityGroup* group)
{
	return EntityGroup_CreateEntity(group);
}

struct Entity* World_CreateEntityInDefaultGroup(World* world)
{
	return world ? World_CreateEntity(world->impl->defaultEntityGroup) : NULL;
}

void World_DestroyEntity(struct Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	EntityImpl* impl = ent->impl;

	if ( impl->prev )
	{
		impl->prev->next = impl->next;
	}
	else
	{
		impl->ownerGroup->head = impl->next;
	}

	if ( impl->next )
	{
		impl->next->prev = impl->prev;
	}
	else
	{
		impl->ownerGroup->tail = impl->prev;
	}

	if ( impl->ownerGroup->count > 0 )
	{
		--impl->ownerGroup->count;
	}

	EntityImpl_Destroy(impl);
}

struct Entity* World_GetEntityGroupHead(struct EntityGroup* group)
{
	return (group && group->head) ? &group->head->entity : NULL;
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

size_t World_GetEntityCount(const struct EntityGroup* group)
{
	return group ? group->count : 0;
}

bool World_BeginLoadEntityGroup(World* world, const char* filePath)
{
	if ( !world )
	{
		TraceLog(LOG_WARNING, "WORLD: Cannot load new entity group into a null world.");
		return false;
	}

	if ( !filePath || !filePath[0] )
	{
		TraceLog(LOG_WARNING, "WORLD: Cannot load new entity group from an invalid file.");
		return false;
	}

	if ( AsyncThreadIsRunning(world->impl) )
	{
		TraceLog(LOG_WARNING, "WORLD: Cannot load new entity group as an async load is already in progress.");
		return false;
	}

	if ( THREADING_GET_SEMAPHORE_VALUE(&world->impl->asyncLoadSemaphore) != ASYNCLOAD_SEM_IDLE)
	{
		TraceLog(LOG_WARNING, "WORLD: Cannot load new entity group as a previous group has not yet been integrated.");
		return false;
	}

	TraceLog(LOG_INFO, "WORLD: [%s] Beginning async load of entities.", filePath);

	CreateAsyncLoadThreadArgs(world->impl, filePath);

	// Increment the semaphore twice.
	// The thread will decrement the semaphore once it's done,
	// and then the main thread will decrement it again once the
	// produced entity group is imported into the world.
	THREADING_INCREMENT_SEMAPHORE(&world->impl->asyncLoadSemaphore);
	THREADING_INCREMENT_SEMAPHORE(&world->impl->asyncLoadSemaphore);
	THREADING_ASSERT_SEMAPHORE_HAS_VALUE(&world->impl->asyncLoadSemaphore, ASYNCLOAD_SEM_BUSY);

	THREADING_CREATE(&world->impl->asyncLoadThread, NULL, &AsyncLoad_Routine, world->impl->asyncLoadThreadArgs);
	return true;
}

bool World_IsLoadingEntityGroup(World* world)
{
	return world ? THREADING_GET_SEMAPHORE_VALUE(&world->impl->asyncLoadSemaphore) != ASYNCLOAD_SEM_IDLE : false;
}

void World_Update(World* world)
{
	if ( !world )
	{
		return;
	}

	if ( AsyncLoadThreadIsFinished(world->impl) )
	{
		IntegrateEntityGroup(world->impl);
	}

	for ( Entity* ent = World_GetEntityGroupHead(world->impl->defaultEntityGroup); ent; ent = World_GetNextEntity(ent) )
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
void World_Render(World* world, struct CameraComponent* camComp)
{
	if ( !world || !camComp )
	{
		return;
	}

	CameraComponentImpl* camImpl = camComp->impl;

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

	for ( Entity* ent = World_GetEntityGroupHead(world->impl->defaultEntityGroup); ent; ent = World_GetNextEntity(ent) )
	{
		EntityImpl_Render(ent->impl, camera);
	}

	if ( Debugging.debuggingEnabled )
	{
		RenderDebug(world, camera);
	}

	EndMode3D();
}
