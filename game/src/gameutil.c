#include <float.h>
#include "gameutil.h"
#include "raymath.h"

Rectangle NormaliseRectangle(Rectangle rect)
{
	if ( rect.width < 0.0f )
	{
		rect.x += rect.width;
		rect.width *= -1.0f;
	}

	if ( rect.height < 0.0f )
	{
		rect.y += rect.height;
		rect.height *= -1.0f;
	}

	return rect;
}

Rectangle ExpandRectangle(Rectangle rect, Vector2 delta)
{
	if ( delta.x < 0.0f )
	{
		rect.x += delta.x;
		rect.width -= delta.x;
	}
	else
	{
		rect.width += delta.x;
	}

	if ( delta.y < 0.0f )
	{
		rect.y += delta.x;
		rect.height -= delta.y;
	}
	else
	{
		rect.height += delta.y;
	}

	return rect;
}

Vector2 Vector2CollapseEpsilon(Vector2 vec)
{
	if ( fabsf(vec.x) < FLT_EPSILON )
	{
		vec.x = 0.0f;
	}

	if ( fabsf(vec.y) < FLT_EPSILON )
	{
		vec.y = 0.0f;
	}

	return vec;
}

Vector2 Vector2SwapComponents(Vector2 vec)
{
	return (Vector2){ vec.y, vec.x };
}

bool Vector2IsZero(Vector2 vec)
{
	return vec.x == 0.0f && vec.y == 0.0f;
}

Vector2 RectangleMin(Rectangle rect)
{
	return (Vector2){ rect.x, rect.y };
}

Vector2 RectangleMax(Rectangle rect)
{
	return (Vector2){ rect.x + rect.width, rect.y + rect.height };
}

// Adapted from https://stackoverflow.com/a/4543530/2054335
bool LinesIntersect(Vector2 p0, Vector2 p1, Vector2 q0, Vector2 q1, float* t, Vector2* point)
{
	// Convert both lines m and n into the form ax + by = c:
	// a = p1.y - p0.y; b = p0.x - p1.x; c = (a * p0.x) + (b * p0.y)

	float a = p1.y - p0.y;
	float b = p0.x - p1.x;
	float c = (a * p0.x) + (b * p0.y);

	float d = q1.y - q0.y;
	float e = q0.x - q1.x;
	float f = (d * q0.x) + (e * q0.y);

	// Calculate determinant
	float determinant = (a * e) - (d * b);

	if ( fabsf(determinant) < FLT_EPSILON )
	{
		// Lines are parallel. This also catches the case
		// where the lines are ill-defined (ie. have length zero),
		// since a and b (or d and e) will be zero.
		return false;
	}

	if ( t || point )
	{
		Vector2 intersection = { ((e * c) - (b * f)) / determinant, ((a * f) - (d * c)) / determinant };

		if ( point )
		{
			*point = intersection;
		}

		if ( t )
		{
			// TODO: Maybe this could be optimised?

			Vector2 pLine = Vector2Subtract(p1, p0);
			float pLength = Vector2Length(pLine);

			Vector2 pLineSegment = Vector2Subtract(intersection, p0);
			float segLength = Vector2Length(pLineSegment);

			// t is the ratio of the two lengths.
			*t = segLength / pLength;

			if ( Vector2DotProduct(pLineSegment, pLine) < 0.0f )
			{
				// Intersection was behind p0, so invert ratio
				*t *= -1.0f;
			}
		}
	}

	return true;
}

bool LineIntersectsRect(Vector2 p0, Vector2 p1, Rectangle rect, float* t)
{
	if ( fabsf(p1.x - p0.x) < FLT_EPSILON && fabsf(p1.y - p0.y) < FLT_EPSILON )
	{
		// Line is a single point - not valid.
		return false;
	}

	rect = NormaliseRectangle(rect);

	float hSlabt0 = 0.0f;
	Vector2 hSlabi0 = { 0.0f, 0.0f };
	float hSlabt1 = 0.0f;
	Vector2 hSlabi1 = { 0.0f, 0.0f };
	bool intersectedHSlab = false;

	float vSlabt0 = 0.0f;
	Vector2 vSlabi0 = { 0.0f, 0.0f };
	float vSlabt1 = 0.0f;
	Vector2 vSlabi1 = { 0.0f, 0.0f };
	bool intersectedVSlab = false;

	// If line is not parallel to any axis, it will intersect horizontal and vertical
	// slabs in two places each. If at least one intersection on either slab is within the bounds
	// of the other slab, the line intersects the rect. We must check both slabs equally to be sure.
	// If the line is parallel to an axis, it will intersect only one slab in two places.
	// The same bounds checks apply.

	// Horizontal slab: y co-ord of line is constant
	intersectedHSlab =
		LinesIntersect(p0, p1, (Vector2){ rect.x, rect.y }, (Vector2){ rect.x + rect.width, rect.y }, &hSlabt0, &hSlabi0) &&
		LinesIntersect(p0, p1, (Vector2){ rect.x, rect.y + rect.height }, (Vector2){ rect.x + rect.width, rect.y + rect.height }, &hSlabt1, &hSlabi1);

	// Vertical slab: x co-ord of line is constant
	intersectedVSlab =
		LinesIntersect(p0, p1, (Vector2){ rect.x, rect.y }, (Vector2){ rect.x, rect.y + rect.height }, &vSlabt0, &vSlabi0) &&
		LinesIntersect(p0, p1, (Vector2){ rect.x + rect.width, rect.y }, (Vector2){ rect.x + rect.width, rect.y + rect.height }, &vSlabt1, &vSlabi1);

	// Find out the minimum t value - this is the first intersection point along the line.
	// Start out with a huge value and trim it down.
	float mintVal = FLT_MAX;

	if ( intersectedHSlab )
	{
		// Is the intersection with the horizontal slab within the X bounds of the vertical slab?
		if ( hSlabi0.x >= rect.x && hSlabi0.x <= rect.x + rect.width )
		{
			if ( hSlabt0 < mintVal )
			{
				mintVal = hSlabt0;
			}
		}

		if ( hSlabi1.x >= rect.x && hSlabi1.x <= rect.x + rect.width )
		{
			if ( hSlabt1 < mintVal )
			{
				mintVal = hSlabt1;
			}
		}
	}

	if ( intersectedVSlab )
	{
		// Is the intersection with the vertical slab within the Y bounds of the horizontal slab?
		if ( vSlabi0.y >= rect.y && vSlabi0.y <= rect.y + rect.height )
		{
			if ( vSlabt0 < mintVal )
			{
				mintVal = vSlabt0;
			}
		}

		if ( vSlabi1.y >= rect.y && vSlabi1.y <= rect.y + rect.height )
		{
			if ( vSlabt1 < mintVal )
			{
				mintVal = vSlabt1;
			}
		}
	}

	if ( mintVal < FLT_MAX )
	{
		if ( t )
		{
			*t = mintVal;
		}

		return true;
	}
	else
	{
		// No intersection.
		return false;
	}
}
