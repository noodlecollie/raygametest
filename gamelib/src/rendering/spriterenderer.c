#include "rendering/spriterenderer.h"
#include "gamelib/external/raylibheaders.h"

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
