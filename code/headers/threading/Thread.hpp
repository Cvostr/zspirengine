#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h> //Include POSIX thread implementation
#endif

namespace Engine {
	class Thread {
	public:
		Thread() : mThreadHandle(nullptr){

		}

		bool Run();
		void Stop();
		bool IsRunning();
		void SetThreadPriority_(int priority);
		virtual void THRFunc() = 0;
	private:
		void* mThreadHandle;
	};
}