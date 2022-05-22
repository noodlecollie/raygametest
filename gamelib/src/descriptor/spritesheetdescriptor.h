#pragma once

#include "gamelib/external/raylibheaders.h"
#include "gamelib/gametypes.h"

typedef struct SpriteSheetDescriptor SpriteSheetDescriptor;
typedef struct SpriteSheetAnimation SpriteSheetAnimation;

SpriteSheetDescriptor* SpriteSheetDescriptor_LoadFromJSON(const char* filePath);
void SpriteSheetDescriptor_Destroy(SpriteSheetDescriptor* descriptor);

SpriteSheetAnimation* SpriteSheetDescriptor_GetAnimation(SpriteSheetDescriptor* descriptor, const char* animName);
SpriteSheetAnimation* SpriteSheetDescriptor_GetFirstAnimation(SpriteSheetDescriptor* descriptor);
SpriteSheetAnimation* SpriteSheetDescriptor_GetNextAnimation(SpriteSheetAnimation* anim);
Texture2D* SpriteSheetDescriptor_GetAnimationTexture(SpriteSheetAnimation* anim);
Vector2i SpriteSheetDescriptor_GetAnimationFrameBounds(SpriteSheetAnimation* anim);
size_t SpriteSheetDescriptor_GetAnimationFrameCount(SpriteSheetAnimation* anim);
