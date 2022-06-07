#include "presets/presetshaders.h"
#include "presets/presetnames.h"
#include "rendering/builtinshaders.h"

static Shader LoadSampleRectShader()
{
	Shader shader = LoadShaderFromMemory(SHD_SAMPLERECT_VS, SHD_SAMPLERECT_FS);

	if ( shader.id == 0 )
	{
		TraceLog(LOG_ERROR, "PRESET SHADERS: Could not load sample rect shader!");
		return shader;
	}

	shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "sampleRect");

	if ( shader.locs[SHADER_LOC_VECTOR_VIEW] >= 0 )
	{
		TraceLog(LOG_WARNING, "PRESET SHADERS: Sample rect shader \"sampleRect\" uniform bound to location %d", shader.locs[SHADER_LOC_VECTOR_VIEW]);
	}
	else
	{
		TraceLog(LOG_WARNING, "PRESET SHADERS: Could not bind \"sampleRect\" uniform for sample rect shader!");
	}

	return shader;
}

Shader PresetShaders_Create(const char* name)
{
	TraceLog(LOG_DEBUG, "PRESET SHADERS: Attempting to load \"%s\"", name ? name : "");

	if ( TextIsEqual(name, PRESET_SHADER_DEFAULT) )
	{
		return LoadShaderFromMemory(SHD_DEFAULT_VS, SHD_DEFAULT_FS);
	}
	else if ( TextIsEqual(name, PRESET_SHADER_SAMPLERECT) )
	{
		return LoadSampleRectShader();
	}
	else
	{
		TraceLog(LOG_ERROR, "PRESET SHADERS: Shader name \"%s\" was not recognised", name ? name : "");
	}

	return (Shader){ 0 };
}
