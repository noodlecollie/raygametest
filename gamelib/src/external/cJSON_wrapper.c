#include "external/cJSON_wrapper.h"
#include "gamelib/memoryallocator.h"
#include "gamelib/external/raylibheaders.h"

// Used for reporting errors.
#define ERROR_SNAPSHOT_SPAN 10
#define TOTAL_ERROR_SNAPSHOT_LENGTH (3 * ERROR_SNAPSHOT_SPAN)

static const char* GetParseErrorSnapshot(const char* origData, const char* error)
{
	static char buffer[TOTAL_ERROR_SNAPSHOT_LENGTH + 1];

	size_t errorBytesIntoData = (size_t)(error - origData);

	size_t numLeadingSpacesRequired = errorBytesIntoData < ERROR_SNAPSHOT_SPAN
		? (ERROR_SNAPSHOT_SPAN - errorBytesIntoData)
		: 0;

	error -= (ERROR_SNAPSHOT_SPAN - numLeadingSpacesRequired);

	size_t index = 0;
	for ( ; index < sizeof(buffer) - 1; ++index )
	{
		if ( numLeadingSpacesRequired > 0 )
		{
			buffer[index] = ' ';
			--numLeadingSpacesRequired;
			continue;
		}

		if ( !(*error) )
		{
			break;
		}

		buffer[index] = *(error++);
	}

	buffer[index] = '\0';

	return buffer;
}

static void ReportParseError(const char* filePath, const char* data)
{
	const char* error = cJSON_GetErrorPtr();

	TraceLog(
		LOG_DEBUG,
		"JSON: Failed to parse %s. Error at character %zu:\n"
		"... %sn"
		"...     Here ^",
		filePath,
		(size_t)(error - data) + 1,
		GetParseErrorSnapshot(data, error)
	);
}

void cJSONWrapper_InitHooks(void)
{
	static cJSON_Hooks hooks = { &GameAlloc, &GameFree };

	cJSON_InitHooks(&hooks);
}

cJSON* cJSONWrapper_ParseFile(const char* filePath)
{
	cJSONWrapper_InitHooks();

	if ( !filePath || !(*filePath) )
	{
		return NULL;
	}

	cJSON* json = NULL;
	unsigned int bytesRead = 0;
	char* data = (char*)LoadFileData(filePath, &bytesRead);

	if ( data && bytesRead > 0 )
	{
		json = cJSON_ParseWithLength(data, (size_t)bytesRead);

		if ( !json )
		{
			ReportParseError(filePath, data);
		}
	}
	else
	{
		TraceLog(
			LOG_DEBUG,
			"JSON: Failed to read file %s from disk, or contents were empty.",
			filePath
		);
	}

	if ( data )
	{
		UnloadFileData((unsigned char*)data);
	}

	return json;
}

cJSON* cJSONWrapper_GetObjectItemOfType(cJSON* object, const char* key, int itemType)
{
	if ( !object )
	{
		return NULL;
	}

	cJSON* item = cJSON_GetObjectItem(object, key);

	return (item && item->type == itemType) ? item : NULL;
}
