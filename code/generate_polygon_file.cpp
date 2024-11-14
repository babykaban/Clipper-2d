/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */
#include "main.h"
#include <time.h>

#include "clipper_math.h"
#include "clipper_math_f64.h"

#define PROFILER 1
#include "profiler.cpp"

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

inline b32
IsCollinear(v2_f64 a, v2_f64 b, v2_f64 c, f64 Epsilon)
{
    f64 Area = fabs((b.y - a.y) * (c.x - b.x) - (c.y - b.y) * (b.x - a.x));
    return(Area < Epsilon);
}

// Function to generate a random float between min and max
inline f64
RandFloat(f64 min, f64 max)
{
    return(min + (f64)rand() / (f64)(RAND_MAX / (max - min)));
}

internal triangle
GenerateRandomTriangle(f64 minX, f64 maxX, f64 minY, f64 maxY, f64 Epsilon)
{
    triangle Result = {};


    do
    {
        // Generate random points within the specified range
        Result.p1.x = RandFloat(minX, maxX);
        Result.p1.y = RandFloat(minY, maxY);

        Result.p2.x = RandFloat(minX, maxX);
        Result.p2.y = RandFloat(minY, maxY);

        Result.p3.x = RandFloat(minX, maxX);
        Result.p3.y = RandFloat(minY, maxY);

    } while (IsCollinear(Result.p1, Result.p2, Result.p3, Epsilon)); // Repeat if points are nearly collinear

    return(Result);
}

inline f64
ProjectPointOnAxis(v2_f64 p, v2_f64 axis)
{
    f64 Result = Inner(p, axis) / SquareRoot(Inner(axis, axis));
    return(Result);
}

// Get the min and max projections of a triangle on an axis
inline void
ProjectTriangleOnAxis(triangle T, v2_f64 axis, f64 *min, f64 *max)
{
    *min = *max = ProjectPointOnAxis(T.p1, axis);

    for(u32 i = 1; i < 3; ++i)
    {
        f64 projection = ProjectPointOnAxis(T.Verts[i], axis);
        if(projection < *min)
            *min = projection;
        if(projection > *max)
            *max = projection;
    }
}

// Check if projections overlap on a given axis
inline b32
ProjectionsOverlap(f64 minA, f64 maxA, f64 minB, f64 maxB)
{
    return !(maxA < minB || maxB < minA);
}

// Check if two triangles overlap using the Separating Axis Theorem
b32
TrianglesOverlap(triangle T0, triangle T1)
{
    // Get the edges of each triangle
    v2_f64 edges0[3] = { T0.p2 - T0.p1, T0.p3 - T0.p2, T0.p1 - T0.p3 };
    v2_f64 edges1[3] = { T1.p2 - T1.p1, T1.p3 - T1.p2, T1.p1 - T1.p3 };

    // Test each edge as a potential separating axis
    for(int i = 0; i < 3; ++i)
    {
        v2_f64 axis1 = { -edges0[i].y, edges0[i].x }; // Perpendicular to edge
        v2_f64 axis2 = { -edges1[i].y, edges1[i].x }; // Perpendicular to edge

        // Project both triangles onto the axis
        f64 min1, max1, min2, max2;

        ProjectTriangleOnAxis(T0, axis1, &min1, &max1);
        ProjectTriangleOnAxis(T1, axis1, &min2, &max2);

        if(!ProjectionsOverlap(min1, max1, min2, max2))
            return 0; // Found a separating axis

        ProjectTriangleOnAxis(T0, axis2, &min1, &max1);
        ProjectTriangleOnAxis(T1, axis2, &min2, &max2);

        if(!ProjectionsOverlap(min1, max1, min2, max2))
            return 0; // Found a separating axis
    }

    // If no separating axis was found, triangles overlap
    return 1;
}

internal triangle
GenerateOverlapTriangleFor(triangle T, f64 minX, f64 maxX, f64 minY, f64 maxY, f64 Epsilon)
{
    triangle Result = {};

    do
    {
        Result = GenerateRandomTriangle(minX, maxX, minY, maxY, Epsilon);

    } while(!TrianglesOverlap(T, Result));

    return(Result);
}

inline void
PrintTriangle(triangle T)
{
    printf("Triangle is: ");

    for(u32 I = 0;
        I < 3;
        ++I)
    {
        printf("(%.2f, %.2f), ", T.Verts[I].x, T.Verts[I].y);
    }

    printf("\n");
}

int
main()
{
    BeginProfile();

    srand((u32)time(0)); // Seed for randomness
    
    u32 Count = 48000000;
    triangle *Tris = (triangle *)malloc(sizeof(triangle)*Count);
    for(u32 I = 0;
        I < Count;
        I += 2)
    {
        Tris[I] = GenerateRandomTriangle(-200.0, 200.0, -200.0, 200.0, 5.0);
        Tris[I + 1] = GenerateOverlapTriangleFor(Tris[I], -200.0, 200.0, -200.0, 200.0, 5.0);
        PrintTriangle(Tris[I]);
        PrintTriangle(Tris[I + 1]);
    }

//    triangle T = GenerateRandomTriangle(-100.0, 100.0, -100.0, 100.0, 3.0);
//    PrintTriangle(T);

//    triangle T0 = GenerateOverlapTriangleFor(T, -100.0, 100.0, -100.0, 100.0, 3.0);
//    PrintTriangle(T0);

    EndAndPrintProfile();
    
    return(0);
}
