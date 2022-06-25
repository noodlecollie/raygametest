#pragma once

#include "gamelib/external/raylibheaders.h"

// Note that if Debugging.debuggingEnabled is not set to true, these functions do nothing.
// Additionally, if Debugging.renderCustomDebugItems is not set to true, no items will be drawn.
// See gamelib/debugging.h for access to these settings.

void DebugRenderCustom_Rectangle(Rectangle rect, Color colour, float lifetime);
