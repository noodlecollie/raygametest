#pragma once

// raylib defined its own bool, which caused me grief.
// Make sure this is included first.
#include <stdbool.h>

// Include this first, to override malloc functions
#include "gamelib/memoryallocator.h"

#include "raylib.h"
#include "raymath.h"
