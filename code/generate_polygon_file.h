#if !defined(GENERATE_POLYGON_FILE_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

union triangle
{
    struct
    {
        v2_f64 p1;
        v2_f64 p2;
        v2_f64 p3;
    };

    v2_f64 Verts[3];
};

struct polygon
{
    u32 Count;
    v2_f64 *Points;
};

struct polygon_set
{
    u32 PolyCount;
    polygon *Polygons;
};

// Function to generate a random float between min and max
inline f64
RandDouble(f64 min, f64 max)
{
    TimeFunction;

    f64 Result = min + (f64)rand() / (f64)(RAND_MAX / (max - min));
    return(Result);
}

inline __m128d
RandDouble_2x(f64 min, f64 max)
{
    TimeFunction;

    f64 D = RAND_MAX / (max - min);
    f64 DInv = 1.0 / D;
    
    __m128d Min_2x = _mm_set1_pd(min);
    __m128d Rand_2x = _mm_mul_pd(_mm_set_pd((f64)rand(), (f64)rand()), _mm_set1_pd(DInv));

    __m128d Result = _mm_add_pd(Min_2x, Rand_2x);

    return(Result);
}

inline __m256d
RandDouble_4x(f64 min, f64 max)
{
    TimeFunction;

    f64 D = RAND_MAX / (max - min);
    f64 DInv = 1.0 / D;
    
    __m256d Min_4x = _mm256_set1_pd(min);
    __m256d Rand_4x = _mm256_mul_pd(_mm256_set_pd((f64)rand(), (f64)rand(), (f64)rand(), (f64)rand()),
                                    _mm256_set1_pd(DInv));

    __m256d Result = _mm256_add_pd(Min_4x, Rand_4x);

    return(Result);
}

// Function to generate a random point within a given bounding box
inline v2_f64
RandomPoint(f64 minX, f64 maxX, f64 minY, f64 maxY)
{
    v2_f64 Result = {};

    Result.x = RandDouble(minX, maxX);
    Result.y = RandDouble(minY, maxY);

    return(Result);
}

#define GENERATE_POLYGON_FILE_H
#endif
