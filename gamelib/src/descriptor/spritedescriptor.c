#include "gamelib/descriptor/spritedescriptor.h"
#include "gamelib/external/raylibheaders.h"
#include "external/cJSON_wrapper.h"

SpriteDescriptor* SpriteDescriptor_Load(const char* filePath)
{
	if ( !filePath || !(*filePath) )
	{
		return NULL;
	}

	cJSON* json = cJSONWrapper_ParseFile(filePath);

	if ( !json )
	{
		TraceLog(LOG_WARNING, "SPRITE DESCRIPTOR: Could not load %s", filePath);
		return NULL;
	}

	// TODO
	cJSON_free(json);
	return NULL;
}
