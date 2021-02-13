#pragma once

#include <string>

namespace Engine {

	enum CpuVendor {
		CPU_VENDOR_INTEL,
		CPU_VENDOR_AMD
	};

	class CpuInfo {
	private:
		uint32_t regs[4];
		void CPUID(unsigned funcId, unsigned subFuncId);
		const uint32_t& EAX() const { return regs[0]; }
		const uint32_t& EBX() const { return regs[1]; }
		const uint32_t& ECX() const { return regs[2]; }
		const uint32_t& EDX() const { return regs[3]; }

		CpuVendor mCpuVendor;

		std::string mCpuModel;

		uint32_t mCoresCount;
		bool mIsHTT;

		bool mHasSSE41;
		bool mHasSSE42;
		bool mHasAVX;
		bool mHasAVX2;
	public:

		CpuVendor GetVendor() { return mCpuVendor; }

		std::string GetCpuModelName() { return mCpuModel; }

		uint32_t GetCoresCount() { return mCoresCount; }
		bool IsCpuHasHTT() { return mIsHTT; }

		bool HasSSE41() { return mHasSSE41; }
		bool HasSSE42() { return mHasSSE42; }

		bool HasAVX() { return mHasAVX; }
		bool HasAVX2() { return mHasAVX2; }

		CpuInfo();

	};
}