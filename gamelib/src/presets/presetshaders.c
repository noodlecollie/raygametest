#include "presets/presetshaders.h"
#include "presets/presetnames.h"
#include "rendering/builtinshaders.h"

Shader PresetShaders_Create(const char* name)
{
	if ( TextIsEqual(name, PRESET_SHADER_DEFAULT) )
	{
		return LoadShaderFromMemory(SHD_DEFAULT_VS, SHD_DEFAULT_FS);
	}

	return (Shader){ 0 };
}
