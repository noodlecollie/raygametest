#include "gamelib/drawingdepth.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gameutil.h"

typedef struct
{
	const char* str;
	DrawingDepth depth;
} NameToDepthMapping;

const NameToDepthMapping MAPPINGS[] =
{
	{ "default", DRAWDEPTH_DEFAULT },
	{ "background", DRAWDEPTH_DEFAULT },
	{ "item", DRAWDEPTH_ITEM },
	{ "player", DRAWDEPTH_PLAYER },
};

bool DrawingDepth_FromString(const char* str, DrawingDepth* out)
{
	if ( !str || !out )
	{
		return false;
	}

	for ( size_t index = 0; index < ARRAY_SIZE(MAPPINGS); ++index )
	{
		if ( TextIsEqual(str, MAPPINGS[index].str) )
		{
			*out = MAPPINGS[index].depth;
			return true;
		}
	}

	return false;
}
