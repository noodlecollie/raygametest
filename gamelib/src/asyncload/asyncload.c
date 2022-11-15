#include "asyncload/asyncload.h"
#include "threading.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/stringutil.h"

AsyncLoadArgs* AsyncLoadArgs_Create(sem_t* semaphore, const char* filePath)
{
	AsyncLoadArgs* args = (AsyncLoadArgs*)MemAlloc(sizeof(AsyncLoadArgs));

	args->semaphore = semaphore;
	args->filePath = DuplicateString(filePath);

	return args;
}

void AsyncLoadArgs_Destroy(AsyncLoadArgs* args)
{
	if ( !args )
	{
		return;
	}

	if ( args->filePath )
	{
		MemFree(args->filePath);
		args->filePath = NULL;
	}

	MemFree(args);
}

void* AsyncLoad_Routine(void* args)
{
	AsyncLoadArgs* inArgs = (AsyncLoadArgs*)args;

	// TODO: Do the thing
	FormatStringBuffer(inArgs->message, sizeof(inArgs->message), "TODO: Do the thing");

	THREADING_DECREMENT_SEMAPHORE(inArgs->semaphore);
	return NULL;
}
