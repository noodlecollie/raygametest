#include <math.h>
#include <float.h>
#include <pthread.h>
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

/*
	THREADING PROTECTION FOR ENTITIES LIST

	This is going to take a little bit of thought, which I don't have the brainpower for
	right now, but I'll make some preliminary notes anyway.

	When we asychronously load a chunk of the world, what race conditions do we want to
	protect against? We want to avoid the worker thread changing any data structures
	from under the feet of the main thread.

	The linked list of entities is an awkward one here, because there are many different
	places from which the data may be accessed. CreateEntity and DestroyEntity are the
	obvious ones, but GetEntityListHead, GetPreviousEntity, GetNextEntity etc. also access
	entities as well, as do calls to Update and Render. Really, anyone from anywhere could
	access the entities list at any time as far as the worker thread is concerned.

	This means it's certainly not a good idea to try and protect the entities list overall
	with a mutex, because this would only protect the head and tail pointers, really - other
	list elements could still be iterated over from anywhere else. The domain of data that
	needs access protection is too large.

	What we could do instead is require that the worker thread does not modify any shared
	data while it is running. It can simply produce a new chain of entities for the list,
	and signal with an atomic primitive when the chain is ready. The chain can then be
	added by acting on the signal from the main thread, mitigating any need for further
	threading protections.

	We will also probably want to *unload* world chunks in the same way. This might be more
	complicated, though, since when loading entities they're "secret" (the main thread
	doesn't know they exist until the signal is raised), but in the unloading case these
	entities are known about and may actively be in use. How do we deal with this?

	Perhaps we need to introduce the concept of a "group", or "chunk", or whatever,
	more formally. Chunks must be added or removed as an atom, and although entities
	within the chunk can be interacted with and referenced by other entities, they can only
	be brought into existence together, or removed from existence together. While it is
	safe to cache a pointer to a non-chunked entity (and react to the entity being destroyed
	via the OnEntityDestroyed callback, for an entity that is a member of a chunk,
	the chunk itself will have a destroy callback called, which represents all entities
	within the chunk. Individual entities within the chunk will *not* have their
	callbacks called, as if there are a whole load of entities then this could block the
	main thread.

	By design, there should not be a large number of chunks in the world - their number
	should be small so that they can be iterated over quickly. They should be considered
	coarse groupings of entities, which have long lifetimes. (Perhaps better to call
	them "scenes", as this better reflects their intended usage? Though it should be
	possible of course to have more than one scene loaded in the world, as this is the
	use case we are catering for...)

	PS. One other thing that hasn't been noted yet, but which is important, is that any
	worker threads must be waited for if the world that owns them is destroyed. This
	will block the main thread, but if it's the world that's being destroyed then that's
	OK because chances are the entire engine is shutting down, and if the user really
	wants to avoid blocking then they can check whether any worker threads still exist
	before they call destroy.
*/
typedef struct
{
	size_t count;
	EntityImpl* head;
	EntityImpl* tail;
} EntityList;

typedef struct WorldImpl
{
	EntityList entities;
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
	EntityImpl* slot = impl->entities.head;

	impl->entities.head = NULL;
	impl->entities.tail = NULL;
	impl->entities.count = 0;

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

	if ( impl->entities.count >= WORLD_MAX_ENTITIES )
	{
		return NULL;
	}

	EntityImpl* slot = (EntityImpl*)MemAlloc(sizeof(EntityImpl));

	slot->ownerWorld = world;
	slot->entity.impl = slot;

	DBL_LL_ADD_TO_TAIL(slot, prev, next, &impl->entities, head, tail);

	++impl->entities.count;

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
		slot->ownerWorld->impl->entities.head = slot->next;
	}

	if ( slot->next )
	{
		slot->next->prev = slot->prev;
	}
	else if ( slot->ownerWorld )
	{
		slot->ownerWorld->impl->entities.tail = slot->prev;
	}

	if ( slot->ownerWorld && slot->ownerWorld->impl->entities.count > 0 )
	{
		--slot->ownerWorld->impl->entities.count;
	}

	EntityImpl_Destroy(slot);
}

struct Entity* World_GetEntityListHead(World* world)
{
	return (world && world->impl->entities.head) ? &world->impl->entities.head->entity : NULL;
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
	return world ? world->impl->entities.count : 0;
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
