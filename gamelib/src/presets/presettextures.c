#include "presets/presettextures.h"
#include "presets/presetnames.h"

static Texture2D CreateDefaultTexture()
{
	Texture2D texture = { 0 };
	Image image = GenImageColor(16, 16, BLACK);

	if ( !image.data )
	{
		TraceLog(LOG_WARNING, "PRESET TEXTURES: Could not create base image for default texture!");
		return texture;
	}

	for ( int y = 0; y < image.height; y += 4 )
	{
		for ( int x = 0; x < image.width; x += 4 )
		{
			if ( ((x / 4) % 2) != ((y / 4) % 2) )
			{
				ImageDrawRectangle(&image, x, y, 4, 4, PINK);
			}
		}
	}

	texture = LoadTextureFromImage(image);
	UnloadImage(image);

	if ( texture.id == 0 )
	{
		TraceLog(LOG_WARNING, "PRESET TEXTURES: Could not create default texture!");
	}

	return texture;
}

Texture2D PresetTextures_Create(const char* presetName)
{
	if ( TextIsEqual(presetName, PRESET_TEXTURE_DEFAULT) )
	{
		return CreateDefaultTexture();
	}

	return (Texture2D){ 0 };
}
