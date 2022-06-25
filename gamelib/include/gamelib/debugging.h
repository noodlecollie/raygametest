#pragma once

#include <stdbool.h>

typedef struct
{
	bool debuggingEnabled;

	bool renderEntityLocations;
	bool renderPhysicsHulls;
	bool renderCustomDebugItems;
} Debugging_Properties;

extern Debugging_Properties Debugging;
