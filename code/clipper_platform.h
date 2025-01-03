#if !defined(CLIPPER_PLATFORM_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

global_variable b32 AVX2_SUPPORTED = false;
global_variable b32 AVX_SUPPORTED = false;
global_variable b32 SSE_SUPPORTED = false;
global_variable b32 SSE2_SUPPORTED = false;
global_variable b32 SSE3_SUPPORTED = false;

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <cpuid.h>
#endif

inline int
check_avx2_support()
{
    int info[4] = {0};

    // Call cpuid with EAX=0 to check the highest function ID
#if defined(_MSC_VER)
    __cpuid(info, 0);
#else
    __cpuid(0, info[0], info[1], info[2], info[3]);
#endif

    // Ensure that the CPU supports function ID 7 (for extended features)
    if (info[0] >= 7) {
#if defined(_MSC_VER)
        __cpuidex(info, 7, 0);
#else
        __cpuid_count(7, 0, info[0], info[1], info[2], info[3]);
#endif
        return (info[1] & (1 << 5)) != 0;  // AVX2 is bit 5 in EBX
    }

    return(0);
}

inline int
check_avx_support()
{
    int info[4] = {0};

    // Call cpuid with EAX=1 to get processor feature information
#if defined(_MSC_VER)
    __cpuid(info, 1);
#else
    __cpuid(1, info[0], info[1], info[2], info[3]);
#endif

    // Check for AVX support (bit 28 of ECX) and OSXSAVE (bit 27 of ECX)
    if ((info[2] & (1 << 28)) && (info[2] & (1 << 27))) {
        // Check if the OS supports XSAVE/XRESTOR
        unsigned long long xcrFeatureMask = 0;
#if defined(_MSC_VER)
        xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
#else
        __asm__ volatile(".byte 0x0f, 0x01, 0xd0" : "=a"(xcrFeatureMask) : "c"(0) : "%edx");
#endif
        // AVX requires that the OS supports saving YMM registers (bit 1 of XCR0)
        return (xcrFeatureMask & 0x6) == 0x6;
    }

    return 0;
}
inline int
check_sse_support()
{
    int info[4] = {0};

#if defined(_MSC_VER)
    __cpuid(info, 1);
#else
    __cpuid(1, info[0], info[1], info[2], info[3]);
#endif

    return((info[3] & (1 << 25)) != 0);  // SSE is bit 25 in EDX
}

inline int
check_sse2_support()
{
    int info[4] = {0};

#if defined(_MSC_VER)
    __cpuid(info, 1);
#else
    __cpuid(1, info[0], info[1], info[2], info[3]);
#endif

    return((info[3] & (1 << 26)) != 0);  // SSE2 is bit 26 in EDX
}

inline int
check_sse3_support()
{
    int info[4] = {0};

#if defined(_MSC_VER)
    __cpuid(info, 1);
#else
    __cpuid(1, info[0], info[1], info[2], info[3]);
#endif

    return((info[2] & (1 << 0)) != 0);   // SSE3 is bit 0 in ECX
}


#define CLIPPER_PLATFORM_H
#endif
