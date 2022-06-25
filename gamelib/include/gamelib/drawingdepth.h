#pragma once

#include <stdint.h>
#include <stdbool.h>

// Higher value = closer to camera
typedef int32_t DrawingDepth;

#define DRAWDEPTH_DEFAULT 0
#define DRAWDEPTH_BACKGROUND 1
#define DRAWDEPTH_TERRAIN 2
#define DRAWDEPTH_ITEM 3
#define DRAWDEPTH_PLAYER 4
#define DRAWDEPTH_GENERAL_MAX 100
#define DRAWDEPTH_DEBUG (DRAWDEPTH_GENERAL_MAX + 1)

bool DrawingDepth_FromString(const char* str, DrawingDepth* out);
