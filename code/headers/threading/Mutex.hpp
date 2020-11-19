#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h> //Include POSIX thread implementation
#endif

namespace Engine {
	class Mutex {
	public:
		Mutex();
		~Mutex();

		void Lock();
		void Release();

	private:
		void* mMutexHandle;
	};
}