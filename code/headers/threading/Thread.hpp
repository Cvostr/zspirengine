#pragma once

namespace Engine {
	class Thread {
	protected:
		bool mShouldRun;
	public:
		Thread() : mThreadHandle(nullptr),
			mShouldRun(false){

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