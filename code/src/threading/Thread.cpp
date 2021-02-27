#include <threading/Thread.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h> //Include POSIX thread implementation
#endif

using namespace Engine;

#ifdef _WIN32
static DWORD WINAPI CallThreadFuncton(VOID* data)
{
	Thread* thread = static_cast<Thread*>(data);
	thread->THRFunc();
	return 0;
}
#endif

#ifdef __linux__
static void* CallThreadFuncton(void* data)
{
	Thread* thread = static_cast<Thread*>(data);
	thread->THRFunc();
	pthread_exit((void*)nullptr);
	return nullptr;
}
#endif

bool Thread::Run() {
	if (mThreadHandle != nullptr)
		return false;
#ifdef _WIN32
	mThreadHandle = CreateThread(nullptr, 0, CallThreadFuncton, this, 0, nullptr);
#else
	mThreadHandle = new pthread_t;
	pthread_attr_t type;
	pthread_attr_init(&type);
	pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);
	pthread_create((pthread_t*)mThreadHandle, &type, CallThreadFuncton, this);
#endif
	mShouldRun = true;
	return true;
}
void Thread::Stop() {
	mShouldRun = false;
#ifdef _WIN32
	WaitForSingleObject((HANDLE)mThreadHandle, INFINITE);
	CloseHandle((HANDLE)mThreadHandle);
#else
	auto* thread = (pthread_t*)mThreadHandle;
	if (thread)
		pthread_join(*thread, nullptr);
#endif
	mThreadHandle = nullptr;
}
bool Thread::IsRunning() {
	return mThreadHandle != nullptr;
}
void Thread::SetThreadPriority_(int priority) {
	if (!IsRunning())
		return;
#ifdef _WIN32
	SetThreadPriority((HANDLE)mThreadHandle, priority);
#else
#endif
}