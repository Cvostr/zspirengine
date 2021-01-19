#pragma once


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