#pragma once

struct ResourcePoolTexture;

typedef struct SpriteSheetDescriptor SpriteSheetDescriptor;

SpriteSheetDescriptor* SpriteSheetDescriptor_LoadFromJSON(const char* filePath);
void SpriteSheetDescriptor_Destroy(SpriteSheetDescriptor* descriptor);

struct ResourcePoolTexture* SpriteSheetDescriptor_GetFrame(SpriteSheetDescriptor* descriptor);
