#pragma once

#include <stdlib.h>

#define DBL_LL_ADD_TO_TAIL(itemPtr, mPrev, mNext, listPtr, mHead, mTail) \
do \
{ \
	((itemPtr)->mPrev) = ((listPtr)->mTail); \
	((itemPtr)->mNext) = NULL; \
	if ( ((listPtr)->mTail) ) \
	{ \
		(((listPtr)->mTail)->mNext) = (itemPtr); \
	} \
	((listPtr)->mTail) = (itemPtr); \
	if ( !((listPtr)->mHead) ) \
	{ \
		((listPtr)->mHead) = (itemPtr); \
	} \
} \
while ( 0 )

#define DBL_LL_REMOVE(itemPtr, mPrev, mNext, listPtr, mHead, mTail) \
do \
{ \
	if ( ((itemPtr)->mPrev) ) \
	{ \
		(((itemPtr)->mPrev)->mNext) = ((itemPtr)->mNext); \
	} \
	else \
	{ \
		((listPtr)->mHead) = ((itemPtr)->mNext); \
	} \
	if ( ((itemPtr)->mNext) ) \
	{ \
		(((itemPtr)->mNext)->mPrev) = ((itemPtr)->mPrev); \
	} \
	else \
	{ \
		((listPtr)->mTail) = ((itemPtr)->mPrev); \
	} \
	((itemPtr)->mPrev) = NULL; \
	((itemPtr)->mNext) = NULL; \
} \
while ( 0 )
