#include "../../headers/threading/Thread.hpp"

using namespace Engine;

static DWORD WINAPI CallThreadFuncton(VOID* data)
{
	Thread* thread = static_cast<Thread*>(data);
	thread->THRFunc();
	return 0;
}

bool Thread::Run() {
	if (mThreadHandle != nullptr)
		return false;
#ifdef _WIN32
	mThreadHandle = CreateThread(nullptr, 0, CallThreadFuncton, this, 0, nullptr);
#else
#endif
	return true;
}
void Thread::Stop() {
#ifdef _WIN32
	WaitForSingleObject((HANDLE)mThreadHandle, INFINITE);
	CloseHandle((HANDLE)mThreadHandle);
#else

#endif
	mThreadHandle = nullptr;
}
bool Thread::IsRunning() {
	return mThreadHandle != nullptr;
}
void Thread::SetThreadPriority_(int priority) {
#ifdef _WIN32
	SetThreadPriority((HANDLE)mThreadHandle, priority);
#else
#endif
}