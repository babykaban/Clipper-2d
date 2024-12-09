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
    v2_f32 *Points;
};

struct polygon_set
{
    u32 PolyCount;
    polygon *Polygons;
};

#include "math_f32.h"

// Function to generate a random float between min and max
inline f64
RandDouble(f64 min, f64 max)
{
    TimeFunction;

    f64 Result = min + (f64)rand() / (f64)(RAND_MAX / (max - min));
    return(Result);
}

inline f32
RandFloat(f32 min, f32 max)
{
    TimeFunction;

    f32 D = RAND_MAX / (max - min);
    f32 DInv = 1.0f / D;

    f32 Result = min + ((f32)rand()*DInv);
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
inline v2_f64
RandomPoint(f64 minX, f64 maxX, f64 minY, f64 maxY)
{
    v2_f64 Result = {};

    Result.x = RandDouble(minX, maxX);
    Result.y = RandDouble(minY, maxY);

    return(Result);
}

inline v2_f32
RandomPoint(f32 minX, f32 maxX, f32 minY, f32 maxY)
{
    v2_f32 Result = {};

    Result.x = RandFloat(minX, maxX);
    Result.y = RandFloat(minY, maxY);

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
PrintPoly(u32 Count, f32 *X, f32 *Y, s32 *Indecies, u32 Identifier = 0)
{

    printf("Polygon[%d] points: ", Identifier);

    for(u32 I = 0;
        I < Count;
        ++I)
    {
        printf("(%.4f, %.4f), ", X[Indecies[I]], Y[Indecies[I]]);
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
