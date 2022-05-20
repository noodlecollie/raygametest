#pragma once

#include <stdbool.h>
#include "gamelib/external/raylibheaders.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

char* DuplicateString(const char* in);

// Ensures that x and y are the minimum point of the rect, and width and height are non-negative.
Rectangle NormaliseRectangle(Rectangle rect);

// Moves the rect origin in negative directions, and expands the
// width and height in positive directions.
Rectangle ExpandRectangle(Rectangle rect, Vector2 delta);

bool Vector2IsZero(Vector2 vec);
Vector2 Vector2PerpendicularClockwise(Vector2 vec);
Vector2 Vector2PerpendicularCounterClockwise(Vector2 vec);
Vector2 Vector2CollapseEpsilonComponents(Vector2 vec);
Vector2 Vector2RoundToIntegers(Vector2 vec);
Vector2 Vector2ProjectAlongSurface(Vector2 delta, Vector2 surfaceNormal);

// Assumes the rect is normalised.
Vector2 RectangleMin(Rectangle rect);
Vector2 RectangleMax(Rectangle rect);
Vector2 RectangleMid(Rectangle rect);

bool RectangleHasNoArea(Rectangle rect);
bool RectangleIsNull(Rectangle rect);

// If t is not null, it will be set to a value indicating
// how far allong the first line the intersection occurred. 0 = at p0,
// 1 = at p1, other values imply points inside or outside this range.
bool LinesIntersect(Vector2 p0, Vector2 p1, Vector2 q0, Vector2 q1, float* t, Vector2* point);

// If t is not null, it will be set to a value indicating
// how far allong the line the intersection occurred. 0 = at p0,
// 1 = at p1, other values imply points inside or outside this range.
// The earliest intersection point along the line is returned.
bool LineIntersectsRect(Vector2 p0, Vector2 p1, Rectangle rect, float* t, Vector2* point, Vector2* normal);

// Returns true if the moving rect collided with the static rect, and false otherwise.
// If the rects collided and contact is not null, it is set to the position of contact.
// Assumes both rectangles are normalised!
bool RectSweep(Rectangle movingRect, Rectangle staticRect, Vector2 delta, Rectangle* contact, Vector2* contactNormal, float* fraction);

// Returns the normal of the edge of the rectangle hit by the line from the middle of the rectangle
// that passes through this point. If the supplied point *is* the middle of the rectangle, or the
// rectangle has no area, returns (0, 0).
Vector2 RectEdgeNormalForPoint(Rectangle rect, Vector2 point);
