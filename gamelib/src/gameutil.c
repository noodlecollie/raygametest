#include <float.h>
#include <stdlib.h>
#include "gamelib/gameutil.h"
#include "gamelib/external/raylibheaders.h"

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
		rect.y += delta.y;
		rect.height -= delta.y;
	}
	else
	{
		rect.height += delta.y;
	}

	return rect;
}

bool Vector2IsZero(Vector2 vec)
{
	return fabsf(vec.x) < FLT_EPSILON && fabsf(vec.y) < FLT_EPSILON;
}

Vector2 Vector2PerpendicularClockwise(Vector2 vec)
{
	return (Vector2){ vec.y, -vec.x };
}

Vector2 Vector2PerpendicularCounterClockwise(Vector2 vec)
{
	return (Vector2){ -vec.y, vec.x };
}

Vector2 Vector2CollapseEpsilonComponents(Vector2 vec)
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

Vector2 Vector2RoundToIntegers(Vector2 vec)
{
	return (Vector2){ roundf(vec.x), roundf(vec.y) };
}

Vector2 Vector2ProjectAlongSurface(Vector2 delta, Vector2 surfaceNormal)
{
	const Vector2 parallelDir = Vector2PerpendicularClockwise(surfaceNormal);
	const float dotProduct = Vector2DotProduct(delta, parallelDir);
	return Vector2Scale(parallelDir, dotProduct);
}

Vector2 RectangleMin(Rectangle rect)
{
	return (Vector2){ rect.x, rect.y };
}

Vector2 RectangleMax(Rectangle rect)
{
	return (Vector2){ rect.x + rect.width, rect.y + rect.height };
}

Vector2 RectangleMid(Rectangle rect)
{
	return (Vector2){ rect.x + (rect.width / 2.0f), rect.y + (rect.height / 2.0f) };
}

bool RectangleHasNoArea(Rectangle rect)
{
	return fabsf(rect.width) < FLT_EPSILON || fabsf(rect.height) < FLT_EPSILON;
}

bool RectangleIsNull(Rectangle rect)
{
	return Vector2IsZero((Vector2){ rect.width, rect.height });
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

bool LineIntersectsRect(Vector2 p0, Vector2 p1, Rectangle rect, float* t, Vector2* point, Vector2* normal)
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
	// Note that in a very literal corner condition, where the intersection lies exactly
	// on the rectangle corner, two t values could be the same. To cater for this, we also
	// check the direction of the edge normal against the direction of the line, and choose
	// the t value whose normal direction is counter to the direction of the line.
	float mintVal = FLT_MAX;
	Vector2* minItsPoint = NULL;
	Vector2 contactNormal = { 0.0f, 0.0f };

	if ( intersectedHSlab )
	{
		// Is the intersection with the horizontal slab within the X bounds of the vertical slab?
		if ( hSlabi0.x >= rect.x && hSlabi0.x <= rect.x + rect.width )
		{
			if ( hSlabt0 < mintVal || (fabsf(mintVal - hSlabt0) < FLT_EPSILON && Vector2DotProduct((Vector2){ 0.0f, -1.0f }, Vector2Subtract(p1, p0)) < 0.0f) )
			{
				mintVal = hSlabt0;
				minItsPoint = &hSlabi0;
				contactNormal = (Vector2){ 0.0f, -1.0f };
			}
		}

		if ( hSlabi1.x >= rect.x && hSlabi1.x <= rect.x + rect.width )
		{
			if ( hSlabt1 < mintVal || (fabsf(mintVal - hSlabt1) < FLT_EPSILON && Vector2DotProduct((Vector2){ 0.0f, 1.0f }, Vector2Subtract(p1, p0)) < 0.0f) )
			{
				mintVal = hSlabt1;
				minItsPoint = &hSlabi1;
				contactNormal = (Vector2){ 0.0f, 1.0f };
			}
		}
	}

	if ( intersectedVSlab )
	{
		// Is the intersection with the vertical slab within the Y bounds of the horizontal slab?
		if ( vSlabi0.y >= rect.y && vSlabi0.y <= rect.y + rect.height )
		{
			if ( vSlabt0 < mintVal || (fabsf(mintVal - vSlabt0) < FLT_EPSILON && Vector2DotProduct((Vector2){ -1.0f, 0.0f }, Vector2Subtract(p1, p0)) < 0.0f) )
			{
				mintVal = vSlabt0;
				minItsPoint = &vSlabi0;
				contactNormal = (Vector2){ -1.0f, 0.0f };
			}
		}

		if ( vSlabi1.y >= rect.y && vSlabi1.y <= rect.y + rect.height )
		{
			if ( vSlabt1 < mintVal || (fabsf(mintVal - vSlabt1) < FLT_EPSILON && Vector2DotProduct((Vector2){ 1.0f, 0.0f }, Vector2Subtract(p1, p0)) < 0.0f) )
			{
				mintVal = vSlabt1;
				minItsPoint = &vSlabi1;
				contactNormal = (Vector2){ 1.0f, 0.0f };
			}
		}
	}

	if ( minItsPoint )
	{
		if ( t )
		{
			*t = mintVal;
		}

		if ( point )
		{
			*point = *minItsPoint;
		}

		if ( normal )
		{
			*normal = contactNormal;
		}

		return true;
	}
	else
	{
		// No intersection.
		return false;
	}
}

// Adapted from https://noonat.github.io/intersect/#aabb-vs-swept-aabb
bool RectSweep(Rectangle movingRect, Rectangle staticRect, Vector2 delta, Rectangle* contact, Vector2* contactNormal, float* fraction)
{
	staticRect = ExpandRectangle(staticRect, (Vector2){ movingRect.width / 2.0f, movingRect.height / 2.0f });
	staticRect = ExpandRectangle(staticRect, (Vector2){ -movingRect.width / 2.0f, -movingRect.height / 2.0f });

	Vector2 begin = RectangleMid(movingRect);
	Vector2 end = Vector2Add(begin, delta);

	Vector2 normal = { 0.0f, 0.0f };
	Vector2 midpointContact = { 0.0f, 0.0f };
	float contactCoeff = 0.0f;

	if ( CheckCollisionPointRec(begin, staticRect) )
	{
		if ( Vector2Length(Vector2Subtract(end, begin)) < FLT_EPSILON )
		{
			// Sweep begins colliding but has no movement.
			contactCoeff = 1.0f;
			midpointContact = begin;
			normal = Vector2Zero();
		}
		else
		{
			// Sweep begins colliding. Flip the direction and see where the rectangle is intersected with.
			LineIntersectsRect(end, begin, staticRect, &contactCoeff, &midpointContact, &normal);
			contactCoeff = 1.0f - contactCoeff;
		}
	}
	else if ( !LineIntersectsRect(begin, end, staticRect, &contactCoeff, &midpointContact, &normal) || contactCoeff < 0.0f || contactCoeff > 1.0f )
	{
		// No collision.
		return false;
	}

	if ( fraction )
	{
		*fraction = contactCoeff;
	}

	if ( contact )
	{
		contact->x = midpointContact.x - (movingRect.width / 2.0f);
		contact->y = midpointContact.y - (movingRect.height / 2.0f);
		contact->width = movingRect.width;
		contact->height = movingRect.height;
	}

	if ( contactNormal )
	{
		*contactNormal = normal;
	}

	return true;
}

Vector2 RectEdgeNormalForPoint(Rectangle rect, Vector2 point)
{
	if ( fabsf(rect.width) < FLT_EPSILON || fabsf(rect.height) < FLT_EPSILON )
	{
		return (Vector2){ 0.0f, 0.0f };
	}

	Vector2 mid = RectangleMid(rect);
	Vector2 line = Vector2Subtract(point, mid);

	// Correct for a non-square rectangle.
	float distortion = rect.height / rect.width;
	line.x *= distortion;

	if ( Vector2Length(line) < FLT_EPSILON )
	{
		return (Vector2){ 0.0f, 0.0f };
	}

	if ( line.x > line.y )
	{
		return (Vector2){ line.x > 0 ? 1.0f : -1.0f, 0.0f };
	}
	else
	{
		return (Vector2){ 0.0f, line.y > 0 ? 1.0f : -1.0f };
	}
}
