#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
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

#ifdef _MSC_VER
	strcpy_s(dest, destSize, source);
#else
	strncpy(dest, source, destSize);

	// Cheap way to stay safe:
	dest[destSize - 1] = '\0';
#endif
}

void FormatStringBuffer(char* dest, size_t destSize, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	FormatStringBufferV(dest, destSize, format, args);
	va_end(args);
}

void FormatStringBufferV(char* dest, size_t destSize, const char* format, va_list args)
{
	if ( !dest || destSize < 1 )
	{
		return;
	}

#ifdef _MSC_VER
	vsprintf_s(dest, destSize, format, args);
#else
	vsnprintf(dest, destSize, format, args);

	// Cheap way to stay safe:
	dest[destSize - 1] = '\0';
#endif
}

const char* FirstNonWhitespace(const char* str)
{
	if ( !str )
	{
		return NULL;
	}

	while ( *str )
	{
		if ( !isspace(*str) )
		{
			break;
		}
	}

	return str;
}
