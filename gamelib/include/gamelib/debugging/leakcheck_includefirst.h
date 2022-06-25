#pragma once

#ifdef _DEBUG
	#ifdef _MSC_VER
		// Instructions available at https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2022
		#define _CRTDBG_MAP_ALLOC
		#include <stdlib.h>
		#include <crtdbg.h>

		#define LEAKCHECK_INIT() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
		#define LEAKCHECK_DUMP() _CrtDumpMemoryLeaks()
	#else
		#error Memory leak checking is not yet implemented for this platform.
	#endif // _MSC_VER
#else
	#define LEAKCHECK_INIT()
	#define LEAKCHECK_DUMP()
#endif // _DEBUG
