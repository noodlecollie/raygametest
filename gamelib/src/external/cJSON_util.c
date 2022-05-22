#include "external/cJSON_util.h"

bool cJSONUtil_IsVector2(cJSON* item)
{
	if ( !item || item->type != cJSON_Array )
	{
		return false;
	}

	size_t itemIndex = 0;
	for ( cJSON* child = item->child; child; child = child->next, ++itemIndex )
	{
		if ( itemIndex >= 2 || child->type != cJSON_Number )
		{
			return false;
		}
	}

	return itemIndex == 2;
}

bool cJSONUtil_GetVector2Item(cJSON* object, const char* key, Vector2* out)
{
	if ( !object || !key || !out )
	{
		return false;
	}

	cJSON* item = cJSONWrapper_GetObjectItemOfType(object, key, cJSON_Array);

	if ( !item || !cJSONUtil_IsVector2(item) )
	{
		return false;
	}

	out->x = (float)item->child->valuedouble;
	out->y = (float)item->child->next->valuedouble;

	return true;
}
