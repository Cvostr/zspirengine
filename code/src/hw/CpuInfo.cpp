#include "../../headers/hw/CpuInfo.hpp"

#include <algorithm>
#ifdef _WIN32
#include "intrin.h"
#endif

static const uint32_t SSE_POS = 0x02000000;
static const uint32_t SSE2_POS = 0x04000000;
static const uint32_t SSE3_POS = 0x00000001;
static const uint32_t SSE41_POS = 0x00080000;
static const uint32_t SSE42_POS = 0x00100000;
static const uint32_t AVX_POS = 0x10000000;
static const uint32_t AVX2_POS = 0x00000020;
static const uint32_t LVL_NUM = 0x000000FF;
static const uint32_t LVL_TYPE = 0x0000FF00;
static const uint32_t LVL_CORES = 0x0000FFFF;

void Engine::CpuInfo::CPUID(unsigned funcId, unsigned subFuncId) {
#ifdef _WIN32
    __cpuidex((int*)regs, (int)funcId, (int)subFuncId);

#else
    asm volatile
        ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
            : "a" (funcId), "c" (subFuncId));
    // ECX is set to zero for CPUID function 4
#endif
}

Engine::CpuInfo::CpuInfo() {
    CPUID(0, 0);
    std::string VendorString;
    VendorString += std::string((const char*)&EBX(), 4);
    VendorString += std::string((const char*)&EDX(), 4);
    VendorString += std::string((const char*)&ECX(), 4);

    std::string upVId = VendorString;
    for_each(upVId.begin(), upVId.end(), [](char& in) { in = ::toupper(in); });

    if (upVId.find("INTEL") != std::string::npos) {
        mCpuVendor = CPU_VENDOR_INTEL;
        CPUID(4, 0);
        mCoresCount = 1 + (EAX() >> 26) & 0x3F;
    }

    if (upVId.find("AMD") != std::string::npos) {
        mCpuVendor = CPU_VENDOR_AMD;
        //Get CPU cores
        CPUID(0x80000000, 0);
        if (EAX() >= 8) {
            CPUID(0x80000008, 0);
            mCoresCount = 1 + (ECX() & 0xFF);
        }
    }

    //Obtain CPU model string
    for (int i = 0x80000002; i < 0x80000005; ++i) {
        CPUID(i, 0);
        mCpuModel += std::string((const char*)&EAX(), 4);
        mCpuModel += std::string((const char*)&EBX(), 4);
        mCpuModel += std::string((const char*)&ECX(), 4);
        mCpuModel += std::string((const char*)&EDX(), 4);
    }
    //Obtain CPU ext set
    CPUID(1, 0);
    mIsHTT = EDX() & AVX_POS;
    mHasSSE41 = ECX() & SSE41_POS;
    mHasSSE42 = ECX() & SSE41_POS;
    mHasAVX = ECX() & AVX_POS;
    CPUID(7, 0);
    mHasAVX2 = EBX() & AVX2_POS;

}