#pragma once

#include <stdint.h>

typedef struct Vector2i
{
	int x;
	int y;
} Vector2i;

typedef uint32_t Mask32;
typedef uint32_t EntityIndex;

#define MASK32_BITS (sizeof(Mask32) * 8)
