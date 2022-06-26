#pragma once

#include <pthread.h>

// These throw fatal errors if their operations fail.
void Threading_LockMutex(pthread_mutex_t* mutex, const char* file, int line);
void Threading_UnlockMutex(pthread_mutex_t* mutex, const char* file, int line);

#define THREADING_LOCK_MUTEX(mutex) Threading_LockMutex((mutex), __FILE__, __LINE__)
#define THREADING_UNLOCK_MUTEX(mutex) Threading_UnlockMutex((mutex), __FILE__, __LINE__)
