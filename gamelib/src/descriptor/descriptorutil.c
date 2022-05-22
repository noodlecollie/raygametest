#include "descriptor/descriptorutil.h"
#include "gamelib/external/raylibheaders.h"

cJSON* Descriptor_GetContent(cJSON* root, const char* descType, int version)
{
	if ( !root || !descType || !(*descType) || !cJSON_IsObject(root) )
	{
		return NULL;
	}

	cJSON* descriptor = cJSONWrapper_GetObjectItemOfType(root, "descriptor", cJSON_String);

	if ( !descriptor || !TextIsEqual(descriptor->valuestring, descType) )
	{
		return NULL;
	}

	cJSON* jsonVersion = cJSONWrapper_GetObjectItemOfType(root, "version", cJSON_Number);

	if ( !jsonVersion || jsonVersion->valueint != version )
	{
		return NULL;
	}

	return cJSONWrapper_GetObjectItemOfType(root, "content", cJSON_Object);
}
