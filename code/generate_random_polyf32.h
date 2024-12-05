#if !defined(GENERATE_RANDOM_POLYF32_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

#include "math_f32.h"

// Function to generate a random float between min and max
inline f32
RandFloat(f32 min, f32 max)
{
    TimeFunction;

    f32 D = RAND_MAX / (max - min);
    f32 DInv = 1.0f / D;

    f32 Result = min + ((f32)rand()*DInv);
    return(Result);
}

inline __m128d
RandFloat_2x(f32 min, f32 max)
{
    TimeFunction;

    f32 D = RAND_MAX / (max - min);
    f32 DInv = 1.0f / D;
    
    __m128d Min_2x = _mm_set1_pd(min);
    __m128d Rand_2x = _mm_mul_pd(_mm_set_pd((f32)rand(), (f32)rand()), _mm_set1_pd(DInv));

    __m128d Result = _mm_add_pd(Min_2x, Rand_2x);

    return(Result);
}

inline f32_4x
RandFloat_4x(f32 min, f32 max)
{
    TimeFunction;

    f32 D = RAND_MAX / (max - min);
    f32 DInv = 1.0f / D;
    
    f32_4x Min_4x = Set1WF4(min);
    f32_4x Rand_4x = MulWF4(Set((f32)rand(), (f32)rand(),
                                (f32)rand(), (f32)rand()),
                            Set1WF4(DInv));

    f32_4x Result = AddWF4(Min_4x, Rand_4x);

    return(Result);
}

inline __m256d
RandFloat1_4x(void)
{
    TimeFunction;

    f32 DInv = 1.0 / RAND_MAX;
    __m256d Rand_4x = _mm256_set_pd((f32)rand(), (f32)rand(), (f32)rand(), (f32)rand());

    __m256d Result = _mm256_mul_pd(Rand_4x, _mm256_set1_pd(DInv));

    return(Result);
}

inline __m128d
RandFloat1_2x(void)
{
    TimeFunction;

    f32 DInv = 1.0 / RAND_MAX;
    __m128d Rand_2x = _mm_set_pd((f32)rand(), (f32)rand());

    __m128d Result = _mm_mul_pd(Rand_2x, _mm_set1_pd(DInv));

    return(Result);
}

inline f32
RandFloat1(void)
{
    TimeFunction;

    f32 DInv = 1.0 / RAND_MAX;
    f32 Rand = (f32)rand();

    f32 Result = Rand*DInv;

    return(Result);
}

// Function to generate a random point within a given bounding box
inline v2_f32
RandomPoint(f32 minX, f32 maxX, f32 minY, f32 maxY)
{
    v2_f32 Result = {};

    Result.x = RandFloat(minX, maxX);
    Result.y = RandFloat(minY, maxY);

    return(Result);
}

internal void
RadixSort(u32 Count, s32 *First, s32 *Temp, f32 *Angles)
{
    s32 *Source = First;
    s32 *Dest = Temp;
    for(u32 ByteIndex = 0;
        ByteIndex < 32;
        ByteIndex += 8)
    {
        u32 SortKeyOffsets[256] = {};

        // NOTE(casey): First pass - count how many of each key
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            u32 RadixValue = SortKeyToU32(Angles[Source[Index]]);
            u32 RadixPiece = (RadixValue >> ByteIndex) & 0xFF;
            ++SortKeyOffsets[RadixPiece];
        }

        // NOTE(casey): Change counts to offsets
        u32 Total = 0;
        for(u32 SortKeyIndex = 0;
            SortKeyIndex < ArrayCount(SortKeyOffsets);
            ++SortKeyIndex)
        {
            u32 KeyCount = SortKeyOffsets[SortKeyIndex];
            SortKeyOffsets[SortKeyIndex] = Total;
            Total += KeyCount;
        }

        // NOTE(casey): Second pass - place elements into the right location
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            u32 RadixValue = SortKeyToU32(Angles[Source[Index]]);
            u32 RadixPiece = (RadixValue >> ByteIndex) & 0xFF;
            Dest[SortKeyOffsets[RadixPiece]++] = Source[Index];
        }

        s32 *SwapTemp = Dest;
        Dest = Source;
        Source = SwapTemp;
    }
}

#define GENERATE_RANDOM_POLYF32_H
#endif
