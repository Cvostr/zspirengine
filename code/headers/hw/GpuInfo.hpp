#pragma once

#include <string>

namespace Engine {

	enum GpuVendor {
		GPU_VENDOR_NVIDIA,
		GPU_VENDOR_AMD,
		GPU_VENDOR_INTEL
	};

	class GpuInfo {
	private:
		GpuVendor mVendor;
		std::string mGpuModel;
		uint32_t mVramAmount;

	public:

	};
}