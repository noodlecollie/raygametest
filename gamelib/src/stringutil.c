#include <string.h>
#include <stdlib.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/stringutil.h"

char* DuplicateString(const char* in)
{
	if ( !in )
	{
		return NULL;
	}

	size_t length = strlen(in) + 1;

	// Do a normal malloc since this doesn't need to be zeroed.
	char* out = malloc(length);

	// This does mean we have to check the return ourselves, though.
	if ( !out )
	{
		TraceLog(LOG_FATAL, "Could not allocate %zu bytes for string duplication", length);
		return NULL;
	}

	memcpy(out, in, length);
	return out;
}

void CopyString(char* dest, size_t destSize, const char* source)
{
	if ( !dest || destSize < 1 )
	{
		return;
	}

	strncpy(dest, source, destSize);

	// Cheap way to stay safe:
	dest[destSize - 1] = '\0';
}
