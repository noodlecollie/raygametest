#include "threading.h"
#include "gamelib/external/raylibheaders.h"

void Threading_LockMutex(pthread_mutex_t* mutex, const char* file, int line)
{
	if ( !mutex )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: Could not lock null mutex!", file, line);
		return;
	}

	const int result = pthread_mutex_lock(mutex);

	if ( result != 0 )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: pthread_mutex_lock returned error code %d for mutex 0x%p!", file, line, result, mutex);
		return;
	}
}

void Threading_UnlockMutex(pthread_mutex_t* mutex, const char* file, int line)
{
	if ( !mutex )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: Could not unlock null mutex!", file, line);
		return;
	}

	const int result = pthread_mutex_unlock(mutex);

	if ( result != 0 )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: pthread_mutex_unlock returned error code %d for mutex 0x%p!", file, line, result, mutex);
		return;
	}
}
