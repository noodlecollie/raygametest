#pragma once

#include <stdbool.h>

typedef struct
{
	bool debuggingEnabled;

	bool renderEntityLocations;
	bool renderPhysicsHulls;
} Debugging_Properties;

extern Debugging_Properties Debugging;
