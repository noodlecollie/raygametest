#include <math.h>
#include "gamelib/parametric.h"
#include "gamelib/gameutil.h"

#define PCLAMP(x) Clamp(x, 0.0f, 1.0f)

// y = (0.5f * sinf( (2 * PI * t) + (-1.0f * M_PI_2) )) + 0.5f  Peak
// y = (0.5f * sinf( (2 * PI * t) + ( 1.0f * M_PI_2) )) + 0.5f  Trough
// y = (0.5f * sinf( (1 * PI * t) + (-1.0f * M_PI_2) )) + 0.5f  Half up
// y = (0.5f * sinf( (1 * PI * t) + ( 1.0f * M_PI_2) )) + 0.5f  Half down
#define UNIT_SINE(t, freq, polarity) ( (0.5f * sinf(((freq) * PI * PCLAMP(t)) + ((polarity) * 0.5f * PI))) + 0.5f )

float Parametric_LinearRampUp(float normCoefficient)
{
	return PCLAMP(normCoefficient);
}

float Parametric_LinearRampDown(float normCoefficient)
{
	return PCLAMP(1.0f - normCoefficient);
}

float Parametric_SinePeak(float t)
{
	return UNIT_SINE(t, 2.0f, -1.0f);
}

float Parametric_SineTrough(float t)
{
	return UNIT_SINE(t, 2.0f, 1.0f);
}

float Parametric_HalfSineUp(float t)
{
	return UNIT_SINE(t, 1.0f, -1.0f);
}

float Parametric_HalfSineDown(float t)
{
	return UNIT_SINE(t, 1.0f, 1.0f);
}

float Parametric_ParabolaEaseInUp(float t)
{
	t = PCLAMP(t);
	return t * t;
}

float Parametric_ParabolaEaseOutUp(float t)
{
	t = PCLAMP(t);
	return (-1.0f * ((t - 1.0f) * (t - 1.0f))) + 1.0f;
}

float Parametric_ParabolaEaseInDown(float t)
{
	t = PCLAMP(t);
	return (-1.0f * t * t) + 1.0f;
}

float Parametric_ParabolaEaseOutDown(float t)
{
	t = PCLAMP(t);
	return (t - 1.0f) * (t - 1.0f);
}

float Parametric_CubicEaseInUp(float t)
{
	t = PCLAMP(t);
	return t * t * t;
}

float Parametric_CubicEaseOutUp(float t)
{
	t = PCLAMP(t);
	return ((t - 1.0f) * (t - 1.0f) * (t - 1.0f)) + 1;
}

float Parametric_CubicEaseInDown(float t)
{
	t = PCLAMP(t);
	return (-1.0f * t * t * t) + 1.0f;
}

float Parametric_CubicEaseOutDown(float t)
{
	t = PCLAMP(t);
	return -1.0f * ((t - 1.0f) * (t - 1.0f) * (t - 1.0f));
}
