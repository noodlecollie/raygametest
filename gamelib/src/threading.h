#pragma once

#include <stdbool.h>

// Time.h is required first in order for semaphore.h not to throw warnings (boo).
#include <time.h>

#include <semaphore.h>
#include <pthread.h>

#ifdef WIN32
#define THREADING_FUNC_CDECL __cdecl
#else
#define THREADING_FUNC_CDECL
#endif

// These throw fatal errors if their operations fail.

// Mutexes:
void Threading_LockMutex(pthread_mutex_t* mutex, const char* file, int line);
void Threading_UnlockMutex(pthread_mutex_t* mutex, const char* file, int line);
void Threading_InitDefaultMutex(pthread_mutex_t* mutex, const char* file, int line);

// Threads:
void Threading_Create(pthread_t* thread, const pthread_attr_t* attr, void* (THREADING_FUNC_CDECL * start) (void*), void* arg, const char* file, int line);
void Threading_Join(pthread_t thread, void** returnValue, const char* file, int line);

// Semaphores:
void Threading_InitSemaphore(sem_t* semaphore, bool isShared, unsigned int value, const char* file, int line);
void Threading_DestroySemaphore(sem_t* semaphore, const char* file, int line);
void Threading_IncrementSemaphore(sem_t* semaphore, const char* file, int line);
void Threading_DecrementSemaphore(sem_t* semaphore, const char* file, int line);
int Threading_GetSemaphoreValue(sem_t* semaphore, const char* file, int line);

#define THREADING_LOCK_MUTEX(mutex) Threading_LockMutex((mutex), __FILE__, __LINE__)
#define THREADING_UNLOCK_MUTEX(mutex) Threading_UnlockMutex((mutex), __FILE__, __LINE__)
#define THREADING_INIT_DEFAULT_MUTEX(mutex) Threading_InitDefaultMutex((mutex), __FILE__, __LINE__)

#define THREADING_CREATE(thread, attr, start, arg) Threading_Create((thread), (attr), (start), (arg), __FILE__, __LINE__)
#define THREADING_JOIN(thread, returnValue) Threading_Join((thread), (returnValue), __FILE__, __LINE__)

#define THREADING_INIT_SEMAPHORE(semaphore, isShared, value) Threading_InitSemaphore((semaphore), (isShared), (value), __FILE__, __LINE__)
#define THREADING_DESTROY_SEMAPHORE(semaphore) Threading_DestroySemaphore((semaphore), __FILE__, __LINE__)
#define THREADING_INCREMENT_SEMAPHORE(semaphore) Threading_IncrementSemaphore((semaphore), __FILE__, __LINE__)
#define THREADING_DECREMENT_SEMAPHORE(semaphore) Threading_DecrementSemaphore((semaphore), __FILE__, __LINE__)
#define THREADING_GET_SEMAPHORE_VALUE(semaphore) Threading_GetSemaphoreValue((semaphore), __FILE__, __LINE__)
