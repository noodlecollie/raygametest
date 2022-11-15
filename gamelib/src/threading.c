#include <errno.h>
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

void Threading_InitDefaultMutex(pthread_mutex_t* mutex, const char* file, int line)
{
	const int result = pthread_mutex_init(mutex, NULL);

	if ( result != 0 )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: pthread_mutex_init returned error code %d!", file, line, result);
		return;
	}
}

void Threading_Create(pthread_t* thread, const pthread_attr_t* attr, void* (THREADING_FUNC_CDECL * start) (void*), void* arg, const char* file, int line)
{
	const int result = pthread_create(thread, attr, start, arg);

	if ( result != 0 )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: pthread_create returned error code %d!", file, line, result);
		return;
	}
}

void Threading_Join(pthread_t thread, void** returnValue, const char* file, int line)
{
	const int result = pthread_join(thread, returnValue);

	if ( result != 0 )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: pthread_join returned error code %d!", file, line, result);
		return;
	}
}

void Threading_InitSemaphore(sem_t* semaphore, bool isShared, unsigned int value, const char* file, int line)
{
	if ( !semaphore )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: Could not init null semaphore!", file, line);
		return;
	}

	const int result = sem_init(semaphore, isShared ? 1 : 0, value);

	if ( result != 0 )
	{
		const int error = errno;
		TraceLog(LOG_FATAL, "THREADING: %s:%d: sem_init returned error code %d!", file, line, error);
		return;
	}
}

void Threading_DestroySemaphore(sem_t* semaphore, const char* file, int line)
{
	if ( !semaphore )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: Could not destroy null semaphore!", file, line);
		return;
	}

	const int result = sem_destroy(semaphore);

	if ( result != 0 )
	{
		const int error = errno;
		TraceLog(LOG_FATAL, "THREADING: %s:%d: sem_destroy returned error code %d!", file, line, error);
		return;
	}
}

void Threading_IncrementSemaphore(sem_t* semaphore, const char* file, int line)
{
	if ( !semaphore )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: Could not post null semaphore!", file, line);
		return;
	}

	const int result = sem_post(semaphore);

	if ( result != 0 )
	{
		const int error = errno;
		TraceLog(LOG_FATAL, "THREADING: %s:%d: sem_post returned error code %d!", file, line, error);
		return;
	}
}

void Threading_DecrementSemaphore(sem_t* semaphore, const char* file, int line)
{
	if ( !semaphore )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: Could not post null semaphore!", file, line);
		return;
	}

	const int result = sem_wait(semaphore);

	if ( result != 0 )
	{
		const int error = errno;
		TraceLog(LOG_FATAL, "THREADING: %s:%d: sem_wait returned error code %d!", file, line, error);
		return;
	}
}

int Threading_GetSemaphoreValue(sem_t* semaphore, const char* file, int line)
{
	if ( !semaphore )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: Could not get value of null semaphore!", file, line);
		return -1;
	}

	// Default value is something that should never occur normally
	int semaphoreValue = -1;

	const int result = sem_getvalue(semaphore, &semaphoreValue);

	if ( result != 0 )
	{
		const int error = errno;
		TraceLog(LOG_FATAL, "THREADING: %s:%d: sem_getvalue returned error code %d!", file, line, error);
		return -1;
	}

	return semaphoreValue;
}

void Threading_AssertSemaphoreHasValue(sem_t* semaphore, int value, const char* file, int line)
{
	const int actualValue = Threading_GetSemaphoreValue(semaphore, file, line);

	if ( actualValue != value )
	{
		TraceLog(LOG_FATAL, "THREADING: %s:%d: Assertion failed: semaphore value %d did not match expected value %d", actualValue, value);
		return;
	}
}
