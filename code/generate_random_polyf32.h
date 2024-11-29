#if !defined(GENERATE_RANDOM_POLYF32_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

struct v2_f32
{
    f32 x;
    f32 y;
};

// Function to generate a random float between min and max
inline f32
RandFloat(f32 min, f32 max)
{
    TimeFunction;

    f32 Result = min + (f32)rand() / (f32)(RAND_MAX / (max - min));
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

inline __m256d
RandFloat_4x(f32 min, f32 max)
{
    TimeFunction;

    f32 D = RAND_MAX / (max - min);
    f32 DInv = 1.0f / D;
    
    __m256d Min_4x = _mm256_set1_pd(min);
    __m256d Rand_4x = _mm256_mul_pd(_mm256_set_pd((f32)rand(), (f32)rand(), (f32)rand(), (f32)rand()),
                                    _mm256_set1_pd(DInv));

    __m256d Result = _mm256_add_pd(Min_4x, Rand_4x);

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

#define GENERATE_RANDOM_POLYF32_H
#endif
