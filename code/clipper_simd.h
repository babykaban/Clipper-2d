#if !defined(CLIPPER_SIMD_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

typedef __m128d f64_2x;

union f64_4x
{
    __m256d Value;
    __m128d E[2];
};

typedef __m128 f32_4x;
typedef __m256 f32_8x;

typedef __m256i s32_8x;

inline f32_8x
Set1WF8(f32 A)
{
    f32_8x Result = _mm256_set1_ps(A);
    return(Result);
}

inline f32_4x
Set1WF4(f32 A)
{
    f32_4x Result = _mm_set1_ps(A);
    return(Result);
}

inline f64_2x
Set1WD2(f64 A)
{
    f64_2x Result = _mm_set1_pd(A);
    return(Result);
}

inline f64_4x
Set1WD4(f64 A)
{
    f64_4x Result = {};
    Result.Value = _mm256_set1_pd(A);
    return(Result);
}

inline f64_4x
SetWD4(f64 A, f64 B, f64 C, f64 D)
{
    f64_4x Result = {};
    Result.Value = _mm256_set_pd(A, B, C, D);
    return(Result);
}

inline f64_4x
SetWF64_2x4(f64_2x A, f64_2x B)
{
    f64_4x Result = {};
    Result.Value = _mm256_set_m128d(A, B);
    return(Result);
}

inline f32_8x
Set(f32 A0, f32 A1, f32 A2, f32 A3,
    f32 A4, f32 A5, f32 A6, f32 A7)
{
    f32_8x Result = _mm256_set_ps(A0, A1, A2, A3,
                                  A4, A5, A6, A7);
    return(Result);
}

inline f32_4x
Set(f32 A0, f32 A1, f32 A2, f32 A3)
{
    f32_4x Result = _mm_set_ps(A0, A1, A2, A3);
    return(Result);
}

inline s32_8x
Set(s32 A0, s32 A1, s32 A2, s32 A3,
    s32 A4, s32 A5, s32 A6, s32 A7)
{
    s32_8x Result = _mm256_set_epi32(A0, A1, A2, A3,
                                     A4, A5, A6, A7);
    return(Result);
}

inline s32_8x
Set1(s32 A)
{
    s32_8x Result = _mm256_set1_epi32(A);
    return(Result);
}

inline void
StoreWF4(f32 *Dest, f32_4x A)
{
    _mm_store_ps(Dest, A);
}

inline void
StoreWF8(f32 *Dest, f32_8x A)
{
    _mm256_store_ps(Dest, A);
}

inline void
StoreWI8(s32 *Dest, s32_8x A)
{
    _mm256_store_si256((__m256i *)Dest, A);
}

inline f32_4x
LoadWF4(f32 *Source)
{
    f32_4x Result = _mm_load_ps(Source);
    return(Result);
}

inline f32_8x
LoadWF8(f32 *Source)
{
    f32_8x Result = _mm256_load_ps(Source);
    return(Result);
}

inline f32_4x
SubWF4(f32_4x A, f32_4x B)
{
    f32_4x Result = _mm_sub_ps(A, B);
    return(Result);
}

inline f32_4x
HaddWF4(f32_4x A, f32_4x B)
{
    f32_4x Result = _mm_hadd_ps(A, B);
    return(Result);
}

inline f32_4x
AddWF4(f32_4x A, f32_4x B)
{
    f32_4x Result = _mm_add_ps(A, B);
    return(Result);
}

inline f32_4x
MulWF4(f32_4x A, f32_4x B)
{
    f32_4x Result = _mm_mul_ps(A, B);
    return(Result);
}

inline f32_8x
SubWF8(f32_8x A, f32_8x B)
{
    f32_8x Result = _mm256_sub_ps(A, B);
    return(Result);
}

inline f32_8x
AddWF8(f32_8x A, f32_8x B)
{
    f32_8x Result = _mm256_add_ps(A, B);
    return(Result);
}

inline f32_8x
MulWF8(f32_8x A, f32_8x B)
{
    f32_8x Result = _mm256_mul_ps(A, B);
    return(Result);
}

inline f32_4x
Atan2WF4(f32_4x A, f32_4x B)
{
    f32_4x Result = _mm_atan2_ps(A, B);
    return(Result);
}

inline f32_8x
Atan2WF8(f32_8x A, f32_8x B)
{
    f32_8x Result = _mm256_atan2_ps(A, B);
    return(Result);
}

inline f32_4x
CosWF4(f32_4x A)
{
    f32_4x Result = _mm_cos_ps(A);
    return(Result);
}

inline f32_8x
CosWF8(f32_8x A)
{
    f32_8x Result = _mm256_cos_ps(A);
    return(Result);
}

inline s32_8x
AddWI8(s32_8x A, s32_8x B)
{
    s32_8x Result = _mm256_add_epi32(A, B);
    return(Result);
}

inline f64_2x
SubWD2(f64_2x A, f64_2x B)
{
    f64_2x Result = _mm_sub_pd(A, B);
    return(Result);
}

inline f64_2x
AddWD2(f64_2x A, f64_2x B)
{
    f64_2x Result = _mm_add_pd(A, B);
    return(Result);
}

inline f64_2x
MulWD2(f64_2x A, f64_2x B)
{
    f64_2x Result = _mm_mul_pd(A, B);
    return(Result);
}

inline f64_4x
SubWD4(f64_4x A, f64_4x B)
{
    f64_4x Result = {};
    Result.Value = _mm256_sub_pd(A.Value, B.Value);
    return(Result);
}

inline f64_4x
AddWD4(f64_4x A, f64_4x B)
{
    f64_4x Result = {};
    Result.Value = _mm256_add_pd(A.Value, B.Value);
    return(Result);
}

inline f64_4x
MulWD4(f64_4x A, f64_4x B)
{
    f64_4x Result = {};
    Result.Value = _mm256_mul_pd(A.Value, B.Value);
    return(Result);
}

#define CLIPPER_SIMD_H
#endif
