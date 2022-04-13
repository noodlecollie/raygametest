#pragma once

#include <stdbool.h>
#include "raylib.h"

#define MAX_OF(a, b) (((a) > (b)) ? (a) : (b))
#define MIN_OF(a, b) (((a) < (b)) ? (a) : (b))
#define CLAMP(min, val, max) (MIN_OF(MAX_OF(val, min), max))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// Ensures that x and y are the minimum point of the rect, and width and height are non-negative.
Rectangle NormaliseRectangle(Rectangle rect);

// Moves the rect origin in negative directions, and expands the
// width and height in positive directions.
Rectangle ExpandRectangle(Rectangle rect, Vector2 delta);

// If either component less than FLT_EPSILON, sets the component to zero.
Vector2 Vector2CollapseEpsilon(Vector2 vec);

Vector2 Vector2SwapComponents(Vector2 vec);
bool Vector2IsZero(Vector2 vec);

// Assumes the rect is normalised.
Vector2 RectangleMin(Rectangle rect);
Vector2 RectangleMax(Rectangle rect);

// If t is not null, it will be set to a value indicating
// how far allong the first line the intersection occurred. 0 = at beginning,
// 1 = at end, other values imply points inside or outside this range.
bool LinesIntersect(Vector2 begin1, Vector2 end1, Vector2 begin2, Vector2 end2, float* t, Vector2* point);

// If t is not null, it will be set to a value indicating
// how far allong the line the intersection occurred. 0 = at p0,
// 1 = at p1, other values imply points inside or outside this range.
// If the line enters and exits the rectangle, the lowest intersection
// point value is returned.
bool LineIntersectsRect(Vector2 p0, Vector2 p1, Rectangle rect, float* t);
