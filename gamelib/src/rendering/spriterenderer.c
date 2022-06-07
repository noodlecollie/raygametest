#include "rendering/spriterenderer.h"
#include "rendering/renderutils.h"
#include "gamelib/external/raylibheaders.h"
#include "resourcepool/resourcepool.h"
#include "presets/presetnames.h"

typedef struct SpriteRenderer
{
	ResourcePoolMesh* mesh;
	ResourcePoolShader* shader;
	Material material;
} SpriteRenderer;

static SpriteRenderer* Renderer = NULL;
static size_t RefCount = 0;

static SpriteRenderer* CreateSpriteRenderer()
{
	SpriteRenderer* renderer = (SpriteRenderer*)MemAlloc(sizeof(SpriteRenderer));

	renderer->mesh = ResourcePool_LoadPresetMeshAndAddRef(PRESET_MESH_QUAD);
	renderer->shader = ResourcePool_LoadPresetShaderAndAddRef(PRESET_SHADER_SAMPLERECT);
	renderer->material = LoadMaterialDefault();

	return renderer;
}

static void DestroySpriteRenderer(SpriteRenderer* renderer)
{
	if ( renderer )
	{
		UnloadMaterial(renderer->material);
		ResourcePool_RemoveShaderRef(renderer->shader);
		ResourcePool_RemoveMeshRef(renderer->mesh);

		MemFree(renderer);
	}
}

void SpriteRenderer_AddRef(void)
{
	if ( (RefCount + 1) < RefCount )
	{
		TraceLog(LOG_FATAL, "SPRITE RENDERER: Ref count overflow!");
		return;
	}

	if ( RefCount < 1 )
	{
		Renderer = CreateSpriteRenderer();
	}

	++RefCount;
}

void SpriteRenderer_RemoveRef(void)
{
	if ( RefCount < 1 )
	{
		TraceLog(LOG_FATAL, "SPRITE RENDERER: Ref count underflow!");
		return;
	}

	--RefCount;

	if ( RefCount < 1 )
	{
		DestroySpriteRenderer(Renderer);
		Renderer = NULL;
	}
}

void SpriteRenderer_DrawSpriteFrame(SpriteSheetAnimation* animation, size_t frame, Vector2 position, Vector2 scale)
{
	if ( !animation )
	{
		return;
	}

	Texture2D* texture = SpriteSheetDescriptor_GetAnimationTexture(animation);

	if ( !texture )
	{
		return;
	}

	size_t frameCount = SpriteSheetDescriptor_GetAnimationFrameCount(animation);

	if ( frameCount < 1 )
	{
		return;
	}

	if ( frame >= frameCount )
	{
		frame = frameCount - 1;
	}

	// Define the source rect from the texture image that we're going to draw.
	Vector2i sourceRectDim = SpriteSheetDescriptor_GetAnimationFrameBounds(animation);
	Rectangle sourceRect = (Rectangle){ (float)(frame * sourceRectDim.x), 0.0f, (float)sourceRectDim.x, (float)sourceRectDim.y };

	// The size of the desination rectangle is affected by the scale.
	Vector2 destRectDim = (Vector2){ (float)sourceRectDim.x * scale.x, (float)sourceRectDim.y * scale.y };

	// This is the point on the source rectangle that should be lined up with the provided position when drawn.
	SpriteSheetDescriptor* sprDesc = SpriteSheetDescriptor_GetAnimationOwner(animation);
	Vector2 sourceOrigin = SpriteSheetDescriptor_GetOrigin(sprDesc);

	// The dest rect position must be adjusted to cater for the source origin.
	// The scale of the sprite affects this adjustment.
	Vector2 destRectPos = Vector2Subtract(position, Vector2Multiply(sourceOrigin, scale));

	// Now we can construct the dest rectangle.
	Rectangle destRect = (Rectangle){ destRectPos.x, destRectPos.y, destRectDim.x, destRectDim.y };

	// It's easier to explicitly do the above than to provide a value for the
	// "origin" argument to this function.
	DrawTexturePro(*texture, sourceRect, destRect, Vector2Zero(), 0.0f, WHITE);
}

void SpriteRenderer_DrawSpriteFrameNew(SpriteSheetAnimation* animation, size_t frame, Vector2 position, Vector2 scale)
{
	if ( !Renderer )
	{
		TraceLog(LOG_ERROR, "SPRITE RENDERER: DrawSpriteFrame() called before renderer system was initialised!");
		return;
	}

	if ( !animation )
	{
		return;
	}

	Shader* shader = ResourcePool_GetShader(Renderer->shader);

	if ( !shader )
	{
		return;
	}

	Texture2D* texture = SpriteSheetDescriptor_GetAnimationTexture(animation);

	if ( !texture )
	{
		return;
	}

	Mesh* mesh = ResourcePool_GetMesh(Renderer->mesh);

	if ( !mesh )
	{
		return;
	}

	size_t frameCount = SpriteSheetDescriptor_GetAnimationFrameCount(animation);

	if ( frameCount < 1 )
	{
		return;
	}

	if ( frame >= frameCount )
	{
		frame = frameCount - 1;
	}

	// Define the source rect from the texture image that we're going to draw.
	Vector2i sourceRectDim = SpriteSheetDescriptor_GetAnimationFrameBounds(animation);
	Rectangle sourceRect = (Rectangle){ (float)(frame * sourceRectDim.x), 0.0f, (float)sourceRectDim.x, (float)sourceRectDim.y };

	// Convert this to a rectangle in OpenGL texture co-ordinate space.
	Rectangle openGLSourceRect = RenderUtils_CalcOpenGLTextureSubRect((Vector2i){ texture->width, texture->height }, sourceRect);

	// This is the point on the source rectangle that should be lined up with the provided position when drawn.
	SpriteSheetDescriptor* sprDesc = SpriteSheetDescriptor_GetAnimationOwner(animation);
	Vector2 sourceOrigin = SpriteSheetDescriptor_GetOrigin(sprDesc);

	// Construct a matrix that represents the sprite mesh's transform.
	// We start out with a translation of (0.5, 0.5) so that the top left
	// corner of the mesh lies at the origin.
	Matrix transform = MatrixTranslate(0.5f, 0.5f, 0.0f);

	// Then we must scale the sprite mesh (which is a unit square) to be the same size as the source rect.
	transform = MatrixMultiply(transform, MatrixScale(sourceRect.width, sourceRect.height, 1.0f));

	// Then translate the mesh so that the point specified as the origin is over the entity's position.
	transform = MatrixMultiply(transform, MatrixTranslate(-sourceOrigin.x, -sourceOrigin.y, 0.0f));

	// Then scale the sprite to the desired size.
	transform = MatrixMultiply(transform, MatrixScale(scale.x, scale.y, 1.0f));

	// Then translate the sprite to the desired position.
	// TODO: Layer position on Z
	transform = MatrixMultiply(transform, MatrixTranslate(position.x, position.y, 0.0f));

	// Set the source rect as a uniform.
	const float sourceRectVec[4] =
	{
		openGLSourceRect.x,
		openGLSourceRect.y,
		openGLSourceRect.width,
		openGLSourceRect.height
	};

	SetShaderValue(*shader, shader->locs[SHADER_LOC_VECTOR_VIEW], sourceRectVec, SHADER_UNIFORM_VEC4);

	Material material = Renderer->material;
	material.shader = *shader;

	SetMaterialTexture(&material, MATERIAL_MAP_DIFFUSE, *texture);

	DrawMesh(*mesh, material, transform);
}
