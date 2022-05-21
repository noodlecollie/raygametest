#pragma once

struct TexturePoolItem;

typedef struct SpriteSheetDescriptor SpriteSheetDescriptor;

SpriteSheetDescriptor* SpriteSheetDescriptor_LoadFromJSON(const char* filePath);
void SpriteSheetDescriptor_Destroy(SpriteSheetDescriptor* descriptor);

struct TexturePoolItem* SpriteSheetDescriptor_GetFrameAndAddRef(SpriteSheetDescriptor* descriptor);
