#if !defined(GENERATE_POLYGON_FILE_H)
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

union v4_f64
{
    struct
    {
        f64 x;
        f64 y;
        f64 z;
        f64 w;
    };

    f64 E[4];
    __m256d W;
};

struct v2_f64p
{
    f64 *x;
    f64 *y;
};

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

inline f32_8x
RandFloat_8x(f32 min, f32 max)
{
    TimeFunction;

    f32 D = RAND_MAX / (max - min);
    f32 DInv = 1.0f / D;
    
    f32_8x Min_8x = Set1(min);
    f32_8x Rand_8x = MulWF8(Set((f32)rand(), (f32)rand(),
                                (f32)rand(), (f32)rand(),
                                (f32)rand(), (f32)rand(),
                                (f32)rand(), (f32)rand()),
                            Set1(DInv));

    f32_8x Result = AddWF8(Min_8x, Rand_8x);

    return(Result);
}

inline __m256d
RandDouble1_4x(void)
{
    TimeFunction;

    f64 DInv = 1.0 / RAND_MAX;
    __m256d Rand_4x = _mm256_set_pd((f64)rand(), (f64)rand(), (f64)rand(), (f64)rand());

    __m256d Result = _mm256_mul_pd(Rand_4x, _mm256_set1_pd(DInv));

    return(Result);
}

inline __m128d
RandDouble1_2x(void)
{
    TimeFunction;

    f64 DInv = 1.0 / RAND_MAX;
    __m128d Rand_2x = _mm_set_pd((f64)rand(), (f64)rand());

    __m128d Result = _mm_mul_pd(Rand_2x, _mm_set1_pd(DInv));

    return(Result);
}

inline f64
RandDouble1(void)
{
    TimeFunction;

    f64 DInv = 1.0 / RAND_MAX;
    f64 Rand = rand();

    f64 Result = Rand*DInv;

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

inline b32
AngleSorter(f64 x0, f64 x1)
{
    b32 Result = false;

    if(x0 < x1)
    {
        Result = true;
    }

    return(!Result);
}

inline void
Swap(u32 *A, u32 *B)
{
    u32 Temp = *B;
    *B = *A;
    *A = Temp;
}

inline u32
SortKeyToU32(r32 SortKey)
{
    // NOTE(casey): We need to turn our 32-bit floating point value
    // into some strictly ascending 32-bit unsigned integer value
    u32 Result = *(u32 *)&SortKey;
    if(Result & 0x80000000)
    {
        Result = ~Result;
    }
    else
    {
        Result |= 0x80000000;
    }

    return(Result);
}

inline void
PrintPoly(u32 Count, v2_f64 *Points, u32 Identifier = 0)
{

    printf("Polygon[%d] points: ", Identifier);

    for(u32 I = 0;
        I < Count;
        ++I)
    {
        printf("(%.4f, %.4f), ", Points[I].x, Points[I].y);
    }

    printf("\n\n");
}

inline void
PrintPoly(u32 Count, v2_f32 *Points, u32 Identifier = 0)
{

    printf("Polygon[%d] points: ", Identifier);

    for(u32 I = 0;
        I < Count;
        ++I)
    {
        printf("(%.4f, %.4f), ", Points[I].x, Points[I].y);
    }

    printf("\n\n");
}

inline void
PrintPoly(u32 Count, f32 *X, f32 *Y, u32 Identifier = 0)
{

    printf("Polygon[%d] points: ", Identifier);

    for(u32 I = 0;
        I < Count;
        ++I)
    {
        printf("(%.4f, %.4f), ", X[I], Y[I]);
    }

    printf("\n\n");
}

inline void
PrintPoly(u32 Count, f64 *X, f64 *Y, u32 Identifier = 0)
{

    printf("Polygon[%d] points: ", Identifier);

    for(u32 I = 0;
        I < Count;
        ++I)
    {
        printf("(%.4f, %.4f), ", X[I], Y[I]);
    }

    printf("\n\n");
}

inline void
PrintPoly(u32 Count, f64 *Axis, u32 Identifier = 0, b32 IsYAxis = false)
{

    if(IsYAxis)
    {
        printf("Polygon[%d] Y coords: ", Identifier);
    }
    else
    {
        printf("Polygon[%d] X coords: ", Identifier);
    }
    
    for(u32 I = 0;
        I < Count;
        ++I)
    {
        printf("%.4f, ", Axis[I]);
    }

    printf("\n\n");
}

#define GENERATE_POLYGON_FILE_H
#endif
