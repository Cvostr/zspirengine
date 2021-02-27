#include <threading/Mutex.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h> //Include POSIX thread implementation
#endif

Engine::Mutex::Mutex() {
#ifdef _WIN32
	mMutexHandle = new CRITICAL_SECTION;
	InitializeCriticalSection((CRITICAL_SECTION*)this->mMutexHandle);
#else
	mMutexHandle = new pthread_mutex_t;
#endif
}
Engine::Mutex::~Mutex() {
#ifdef _WIN32
	DeleteCriticalSection((CRITICAL_SECTION*)this->mMutexHandle);
#else
	pthread_mutex_destroy((pthread_mutex_t*)mMutexHandle);
#endif
	delete mMutexHandle;
}

void Engine::Mutex::Lock() {
#ifdef _WIN32
	EnterCriticalSection((CRITICAL_SECTION*)mMutexHandle);
#else
	pthread_mutex_lock((pthread_mutex_t*)mMutexHandle);
#endif
}
void Engine::Mutex::Release() {
#ifdef _WIN32
	LeaveCriticalSection((CRITICAL_SECTION*)mMutexHandle);
#else
	pthread_mutex_unlock((pthread_mutex_t*)mMutexHandle);
#endif
}