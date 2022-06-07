#include "presets/presetshaders.h"
#include "presets/presetnames.h"
#include "rendering/builtinshaders.h"

Shader PresetShaders_Create(const char* name)
{
	if ( TextIsEqual(name, PRESET_SHADER_DEFAULT) )
	{
		return LoadShaderFromMemory(SHD_DEFAULT_VS, SHD_DEFAULT_FS);
	}
	else if ( TextIsEqual(name, PRESET_SHADER_SAMPLERECT) )
	{
		return LoadShaderFromMemory(SHD_SAMPLERECT_VS, SHD_SAMPLERECT_FS);
	}

	return (Shader){ 0 };
}
