#pragma once

namespace Engine {
	class GpuObject {
	public:
		bool mCreated;

		GpuObject() : mCreated(false) {}
		virtual void Destroy(){}
	};
}