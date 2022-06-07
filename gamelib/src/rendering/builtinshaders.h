#pragma once

// These are based on raylib's default shaders.
extern const char* SHD_DEFAULT_VS;
extern const char* SHD_DEFAULT_FS;

// Allows for a rectangle within a texture to be specified via a 4D vector
// specifying X, Y, width, and height, all in normalised texture co-ordinates.
// Texture co-ordinates for vertices are remapped to this rectangle,
// where (0,0) is the bottom left corner and (1,1) is the top right.
// Set the rect via:
//   shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocationAttrib(shader, "sampleRect");
extern const char* SHD_SAMPLERECT_VS;
extern const char* SHD_SAMPLERECT_FS;
