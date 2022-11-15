#pragma once

#include "threading.h"

// Values for async load semaphore.
// If the semaphore value is ASYNCLOAD_SEM_IDLE, no load has been
// requested and there is no entity group ready.
// If the value is ASYNCLOAD_SEM_READY, a load that was previously
// invoked has finished, and an entity group is ready.
// If the value is ASYNCLOAD_SEM_BUSY, a load is in progress and
// no entity group is currently ready.
// When a load is initiated, the semaphore value is incremented
// by the main thread and also by the worker thread. When the
// worker thread completes the load, it decrements the semaphore.
// At this point, the main thread can finish up any bookkeeping
// for the newly loaded entity group, and wil then decrement the
// semaphore back to 0 again.
#define ASYNCLOAD_SEM_IDLE 0
#define ASYNCLOAD_SEM_READY 1
#define ASYNCLOAD_SEM_BUSY 2

// All data here, including the instance of the struct itself,
// may be assumed to last at least as long as the async thread,
// but should not be deallocated by the async thread.
// If the semaphore value is ASYNCLOAD_SEM_BUSY (as it may be
// assumed to be when the async thread is started, the async
// thread owns the data; otherwise, the main thread owns the data.
typedef struct AsyncLoadArgs
{
	sem_t* semaphore;
	char* filePath;

	// REMOVE ME: Replace with something more useful
	char message[128];
} AsyncLoadArgs;

// These functions should only be called by the thread that currently
// owns the data:
AsyncLoadArgs* AsyncLoadArgs_Create(sem_t* semaphore, const char* filePath);
void AsyncLoadArgs_Destroy(AsyncLoadArgs* args);

// Args passed should be a pointer to AsyncLoadArgs.
void* AsyncLoad_Routine(void* args);
