#pragma once

#include "gamelib/external/raylibheaders.h"
#include "gamelib/gametypes.h"
#include "external/cJSON_wrapper.h"

typedef struct SpriteSheetDescriptor SpriteSheetDescriptor;
typedef struct SpriteSheetAnimation SpriteSheetAnimation;

SpriteSheetDescriptor* SpriteSheetDescriptor_LoadFromJSONFile(const char* filePath);
SpriteSheetDescriptor* SpriteSheetDescriptor_LoadFromJSONObject(cJSON* root);
void SpriteSheetDescriptor_Destroy(SpriteSheetDescriptor* descriptor);

SpriteSheetAnimation* SpriteSheetDescriptor_GetAnimation(SpriteSheetDescriptor* descriptor, const char* animName);
SpriteSheetAnimation* SpriteSheetDescriptor_GetFirstAnimation(SpriteSheetDescriptor* descriptor);
SpriteSheetAnimation* SpriteSheetDescriptor_GetNextAnimation(SpriteSheetAnimation* anim);

SpriteSheetDescriptor* SpriteSheetDescriptor_GetAnimationOwner(SpriteSheetAnimation* anim);
Texture2D* SpriteSheetDescriptor_GetAnimationTexture(SpriteSheetAnimation* anim);
Vector2i SpriteSheetDescriptor_GetAnimationFrameBounds(SpriteSheetAnimation* anim);
size_t SpriteSheetDescriptor_GetAnimationFrameCount(SpriteSheetAnimation* anim);
float SpriteSheetDescriptor_GetAnimationFPS(SpriteSheetAnimation* anim);
Vector2 SpriteSheetDescriptor_GetAnimationOrigin(SpriteSheetAnimation* anim);
