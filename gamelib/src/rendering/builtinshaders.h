#pragma once

// These are based on raylib's default shaders.
extern const char* SHD_DEFAULT_VS;
extern const char* SHD_DEFAULT_FS;

// Allows for a rectangle within a texture to be specified.
// Texture co-ordinates for vertices are remapped to this rectangle,
// where (0,0) is the bottom left corner and (1,1) is the top right.
extern const char* SHD_SAMPLERECT_VS;
extern const char* SHD_SAMPLERECT_FS;
