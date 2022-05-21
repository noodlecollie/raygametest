#pragma once

typedef struct SpriteDescriptor
{
	int dummy;
} SpriteDescriptor;

SpriteDescriptor* SpriteDescriptor_Load(const char* filePath);
