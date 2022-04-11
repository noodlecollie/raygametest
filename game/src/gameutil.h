#pragma once

#define MAX_OF(a, b) (((a) > (b)) ? (a) : (b))
#define MIN_OF(a, b) (((a) < (b)) ? (a) : (b))
#define CLAMP(min, val, max) (MIN_OF(MAX_OF(val, min), max))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define COL_EQUAL(c0, c1) (((c0).r == (c1).r) && ((c0).g == (c1).g) && ((c0).b == (c1).b) && ((c0).a == (c1).a))
#define COL_EQUAL3(c0, c1) (((c0).r == (c1).r) && ((c0).g == (c1).g) && ((c0).b == (c1).b))
