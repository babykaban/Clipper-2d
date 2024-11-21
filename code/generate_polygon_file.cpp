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
#include "pcg_random.cpp"

#include "generate_polygon_file.h"

inline b32
IsCollinear(v2_f64 a, v2_f64 b, v2_f64 c, f64 Epsilon)
{
    f64 Area = fabs((b.y - a.y) * (c.x - b.x) - (c.y - b.y) * (b.x - a.x));
    return(Area < Epsilon);
}

internal triangle
GenerateRandomTriangle(f64 minX, f64 maxX, f64 minY, f64 maxY, f64 Epsilon)
{
    triangle Result = {};

    do
    {
        // Generate random points within the specified range
        Result.p1.x = RandDouble(minX, maxX);
        Result.p1.y = RandDouble(minY, maxY);

        Result.p2.x = RandDouble(minX, maxX);
        Result.p2.y = RandDouble(minY, maxY);

        Result.p3.x = RandDouble(minX, maxX);
        Result.p3.y = RandDouble(minY, maxY);

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
internal b32
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

// Function to calculate the orientation of three points (p, q, r)
// Returns 0 if collinear, 1 if clockwise, 2 if counterclockwise
inline s32
Orientation(v2_f64 p, v2_f64 q, v2_f64 r)
{
    f64 val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (val == 0)
        return 0;

    return((val > 0) ? 1 : 2);
}

// Function to check if point r lies on line segment pq
inline s32
OnSegment(v2_f64 p, v2_f64 r, v2_f64 q)
{
    if(r.x <= Maximum(p.x, q.x) && r.x >= Minimum(p.x, q.x) &&
       r.y <= Maximum(p.y, q.y) && r.y >= Minimum(p.y, q.y))
        return 1;

    return 0;
}

// Function to check if line segments p1q1 and p2q2 intersect
inline s32
DoIntersect(v2_f64 p1, v2_f64 q1, v2_f64 p2, v2_f64 q2)
{
    s32 o1 = Orientation(p1, q1, p2);
    s32 o2 = Orientation(p1, q1, q2);
    s32 o3 = Orientation(p2, q2, p1);
    s32 o4 = Orientation(p2, q2, q1);

    if((o1 != o2) && (o3 != o4))
        return 1;

    // Special Cases (collinear)
    if(o1 == 0 && OnSegment(p1, p2, q1)) return 1;
    if(o2 == 0 && OnSegment(p1, q2, q1)) return 1;
    if(o3 == 0 && OnSegment(p2, p1, q2)) return 1;
    if(o4 == 0 && OnSegment(p2, q1, q2)) return 1;

    return 0;
}

// Comparison function for qsort to sort points by angle
inline s32
ComparePoints(const void* a, const void* b)
{
    v2_f64 *p1 = (v2_f64 *)a;
    v2_f64 *p2 = (v2_f64 *)b;

    if (p1->x < p2->x) return -1;
    if (p1->x > p2->x) return 1;
    return 0; 
}

// Function to generate a random simple polygon with a given number of vertices
// within a bounding box
internal v2_f64 *
GenerateRandomPolygon(s32 VertexCount, f64 MinX, f64 MaxX, f64 MinY, f64 MaxY)
{
    TimeFunction;

    // Allocate memory for the polygon vertices
    f64 VertexCountInv = 1.0 / (f64)VertexCount;

    v2_f64* polygon = (v2_f64*)malloc(VertexCount * sizeof(v2_f64));
    if(polygon == 0)
    {
        return 0; // Memory allocation failed
    }

    {
        TimeBlock("Generating points");
        // Generate random points
        for(s32 i = 0; i < VertexCount; i++)
        {
            polygon[i] = RandomPoint(MinX, MaxX, MinY, MaxY);
        }
    }

    v2_f64 center = {};
    {
        TimeBlock("Calculate Centers");

        // Sort points in counter-clockwise order using a simple wrapping algorithm
        // This helps avoid self-intersections
        for(s32 i = 0; i < VertexCount; i++)
        {
            center += polygon[i];
        }

        center *= VertexCountInv;
    }

    {
        TimeBlock("Compute angles");
    
        // Calculate angle for each point relative to the center
        for(s32 i = 0; i < VertexCount; i++)
        {
            v2_f64 CP = polygon[i] - center;
            f64 angle = atan2(CP.y, CP.x);
            polygon[i].x = angle; // Temporarily store angle in x for sorting
        }
    }

#if 0
    // Sort points by angle
    qsort(polygon, VertexCount, sizeof(v2_f64), ComparePoints);

    // Restore original x values
    for(s32 i = 0; i < VertexCount; i++)
    {
        f64 angle = polygon[i].x;
        polygon[i].x = center.x + cos(angle) * (rand() / (double)RAND_MAX) *
            (MaxX - MinX) / 2;
    }

    // Check for self-intersections and regenerate if found
    b32 hasIntersection = 0;
    for(int i = 0; i < VertexCount - 1; i++)
    {
        for(int j = i + 2; j < VertexCount; j++)
        {
            // Don't check adjacent edges
            if((i == 0) && (j == VertexCount - 1))
                continue;

            if(DoIntersect(polygon[i], polygon[i + 1], polygon[j], polygon[(j + 1) % VertexCount]))
            {
                hasIntersection = 1;
                break;
            }
        }

        if(hasIntersection)
            break;
    }

    if(hasIntersection)
    {
        // Free the current polygon and try generating a new one
        free(polygon);

        return GenerateRandomPolygon(VertexCount, MinX, MaxX, MinY, MaxY);
    }
#endif
    
    return polygon;
}

#if 1
internal v2_f64 *
GenerateRandomPolygonSIMD(s32 VertexCount, f64 MinX, f64 MaxX, f64 MinY, f64 MaxY)
{
    TimeFunction;

    // Allocate memory for the polygon vertices
    f64 VertexCountInv = 1.0 / (f64)VertexCount;

    f64 *polygon_x = (f64 *)malloc(VertexCount*4 * sizeof(f64));
    f64 *polygon_y = (f64 *)malloc(VertexCount*4 * sizeof(f64));

    if((polygon_x == 0) || (polygon_y == 0))
    {
        return 0; // Memory allocation failed
    }

    {
        TimeBlock("Generating points");

        u32 ItterCount = VertexCount;
        f64 *p_x = polygon_x + 0;
        f64 *p_y = polygon_y + 0;

        if(ItterCount >= 4)
        {
            u32 Count4x = ItterCount / 4;
            for(u32 I = 0;
                I < Count4x;
                I++)
            {
                __m256d Rand4_x0 = RandDouble_4x(MinX, MaxX);
                __m256d Rand4_x1 = RandDouble_4x(MinY, MaxY);
                _mm256_store_pd(p_x, Rand4_x0);
                _mm256_store_pd(p_y, Rand4_x1);
                p_x += 4;
                p_y += 4;
            }

            ItterCount %= 4;
        }

        if(ItterCount >= 2)
        {
            u32 Count2x = ItterCount / 2;
            for(u32 I = 0;
                I < Count2x;
                I++)
            {
                __m128d Rand2_x0 = RandDouble_2x(MinX, MaxX);
                __m128d Rand2_x1 = RandDouble_2x(MinY, MaxY);
                _mm_store_pd(polygon_x, Rand2_x0);
                _mm_store_pd(polygon_y, Rand2_x1);

                p_x += 2;
                p_y += 2;
            }

            ItterCount %= 2;
        }

        if(ItterCount > 0)
        {
            v2_f64 P = RandomPoint(MinX, MaxX, MinY, MaxY);
        
        }
#if 0    
        // Generate random points
        for(s32 i = 0; i < VertexCount; i++)
        {
            polygon[i] = RandomPoint(MinX, MaxX, MinY, MaxY);
        }
#endif
    }

    __m256d CenterABCD_x = _mm256_set1_pd(0);
    __m256d CenterABCD_y = _mm256_set1_pd(0);
    {
        TimeBlock("Calculate Centers");

        u32 ItterCount = VertexCount;
        f64 *p_x = polygon_x + 0;
        f64 *p_y = polygon_y + 0;

        __m256d CountInv_4x = _mm256_set1_pd(1.0 / VertexCount);
        for(u32 I = 0; I < ItterCount; ++I)
        {
            __m256d X_ABCD = _mm256_load_pd(p_x);
            __m256d Y_ABCD = _mm256_load_pd(p_y);

            CenterABCD_x = _mm256_add_pd(CenterABCD_x, X_ABCD);
            CenterABCD_y = _mm256_add_pd(CenterABCD_y, Y_ABCD);

            p_x += 4;
            p_y += 4;
        }

        CenterABCD_x = _mm256_mul_pd(CenterABCD_x, CountInv_4x);
        CenterABCD_y = _mm256_mul_pd(CenterABCD_y, CountInv_4x);
    }

    {
        TimeBlock("Compute angles");

        u32 ItterCount = VertexCount;
        f64 *p_x = polygon_x + 0;
        f64 *p_y = polygon_y + 0;

        for(u32 I = 0; I < ItterCount; ++I)
        {
            __m256d X_ABCD = _mm256_load_pd(p_x);
            __m256d Y_ABCD = _mm256_load_pd(p_y);

            __m256d CPX_4x = _mm256_sub_pd(X_ABCD, CenterABCD_x);
            __m256d CPY_4x = _mm256_sub_pd(Y_ABCD, CenterABCD_y);

            __m256d angle_4x = _mm256_atan2_pd(CPY_4x, CPX_4x);
            __m256d cos_4x = _mm256_cos_pd(angle_4x);
            __m256d rand_4x = RandDouble1_4x();
            __m256d HalfD = _mm256_mul_pd(_mm256_set1_pd(0.5), _mm256_set1_pd(MaxX - MinX));

            __m256d Result = _mm256_add_pd(CenterABCD_x,
                                           _mm256_mul_pd(_mm256_mul_pd(
                                                             cos_4x, rand_4x),
                                                         HalfD));
            
            p_x += 4;
            p_y += 4;
        }
    }
    
#if 0    
    // Sort points in counter-clockwise order using a simple wrapping algorithm
    // This helps avoid self-intersections
    v2_f64 center = {};
    for(s32 i = 0; i < VertexCount; i++)
    {
        center += polygon[i];
    }

    center *= VertexCountInv;

    // Calculate angle for each point relative to the center
    for(s32 i = 0; i < VertexCount; i++)
    {
        v2_f64 CP = polygon[i] - center;
        f64 angle = atan2(CP.y, CP.x);
        polygon[i].x = angle; // Temporarily store angle in x for sorting
    }

    // Sort points by angle
    qsort(polygon, VertexCount, sizeof(v2_f64), ComparePoints);

    // Restore original x values
    for(s32 i = 0; i < VertexCount; i++)
    {
        f64 angle = polygon[i].x;
        polygon[i].x = center.x + cos(angle) * (rand() / (double)RAND_MAX) * (MaxX - MinX) / 2;
    }

    // Check for self-intersections and regenerate if found
    b32 hasIntersection = 0;
    for(int i = 0; i < VertexCount - 1; i++)
    {
        for(int j = i + 2; j < VertexCount; j++)
        {
            // Don't check adjacent edges
            if((i == 0) && (j == VertexCount - 1))
                continue;

            if(DoIntersect(polygon[i], polygon[i + 1], polygon[j], polygon[(j + 1) % VertexCount]))
            {
                hasIntersection = 1;
                break;
            }
        }

        if(hasIntersection)
            break;
    }

    if(hasIntersection)
    {
        // Free the current polygon and try generating a new one
        free(polygon);

        return GenerateRandomPolygon(VertexCount, MinX, MaxX, MinY, MaxY);
    }
#endif
    
//    return polygon;
    return 0;
}
#endif

inline v2_f64
GetPolygonCenter(v2_f64 *polygon, s32 VertexCount)
{
    v2_f64 center = {0, 0};
    for(s32 i = 0; i < VertexCount; i++)
    {
        center.x += polygon[i].x;
        center.y += polygon[i].y;
    }

    center.x /= VertexCount;
    center.y /= VertexCount;

    return(center);
}

inline void
JSONWritePoly(FILE *Out, polygon *Poly, b32 Last)
{
    fprintf(Out, "     {\n");
    fprintf(Out, "       \"name\": \"polygon1\",\n");
    fprintf(Out, "       \"points\": [");

    for(u32 i = 0; i < (Poly->Count - 1); ++i)
    {
        fprintf(Out, "[%f, %f], ", Poly->Points[i].x, Poly->Points[i].y);
    }

    fprintf(Out, "[%f, %f]]\n", Poly->Points[Poly->Count - 1].x, Poly->Points[Poly->Count - 1].y);

    if(Last)
    {
        fprintf(Out, "     }\n");
    }
    else
    {
        fprintf(Out, "     },\n");
    }
}

inline void
JSONWritePolyPair(FILE *Out, polygon *S, polygon *C, u32 Index, b32 Last)
{
    fprintf(Out, "     {\n");
    fprintf(Out, "       \"case\": \"Case%d\",\n", Index);
    fprintf(Out, "       \"points\": {\n");
    fprintf(Out, "           \"subject\": [");

    for(u32 i = 0; i < (S->Count - 1); ++i)
    {
        fprintf(Out, "[%f, %f], ", S->Points[i].x, S->Points[i].y);
    }

    fprintf(Out, "[%f, %f]],\n", S->Points[S->Count - 1].x, S->Points[S->Count - 1].y);

    fprintf(Out, "           \"clip\": [");

    for(u32 i = 0; i < (C->Count - 1); ++i)
    {
        fprintf(Out, "[%f, %f], ", C->Points[i].x, C->Points[i].y);
    }

    fprintf(Out, "[%f, %f]]\n", C->Points[C->Count - 1].x, C->Points[C->Count - 1].y);

    fprintf(Out, "       }\n");
    if(Last)
    {
        fprintf(Out, "     }\n");
    }
    else
    {
        fprintf(Out, "     },\n");
    }
}

internal void
WritePolygonsToJSON(polygon_set *Ss, polygon_set *Cs, char *FileName)
{
    TimeFunction;
    
    FILE *Out;

    fopen_s(&Out, FileName, "w");
    if(Out)
    {
        fprintf(Out, "{\n");
        fprintf(Out, "    \"polygons\": [\n");

        for(u32 I = 0;
            I < (Ss->PolyCount - 1);
            ++I)
        {
            polygon *S = Ss->Polygons + I;
            polygon *C = Cs->Polygons + I;

            JSONWritePolyPair(Out, S, C, I, false);
        }

        JSONWritePolyPair(Out, Ss->Polygons + (Ss->PolyCount - 1),
                          Cs->Polygons + (Cs->PolyCount - 1), Ss->PolyCount - 1, true);
        
        fprintf(Out, "    ]\n");
        fprintf(Out, "}\n");
    }

    fclose(Out);
}

int
main()
{
    BeginProfile();

    srand((u32)time(0)); // Seed for randomness

    __m256d a = _mm256_set_pd(1, 2, 3, 4);
    __m256d b = _mm256_set_pd(5, 6, 7, 8);
    __m256d r = _mm256_hadd_pd(a, b);
    
#if 0    
    u32 Count = 20;
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
#endif

    f64 MaxY = 1000;
    f64 MaxX = 1000;

//    s32 PolygonCount = 1000000;
    s32 PolygonCount = 1;
    s32 numVertices = 100;

    polygon_set SubjectSet = {};
    SubjectSet.PolyCount = PolygonCount;
    SubjectSet.Polygons = (polygon *)malloc(sizeof(polygon)*PolygonCount);

    GenerateRandomPolygon(numVertices, -MaxX, MaxX, -MaxY, MaxY);
    GenerateRandomPolygonSIMD(numVertices, -MaxX, MaxX, -MaxY, MaxY);
    
#if 0
    for(s32 I = 0;
        I < PolygonCount;
        ++I)
    {
        polygon *Poly = SubjectSet.Polygons + I;
        Poly->Count = numVertices;
        Poly->Points = GenerateRandomPolygon(numVertices, -MaxX, MaxX, -MaxY, MaxY);
    }

    polygon_set ClipSet = {};
    ClipSet.PolyCount = PolygonCount;
    ClipSet.Polygons = (polygon *)malloc(sizeof(polygon)*PolygonCount);

    for(s32 I = 0;
        I < PolygonCount;
        ++I)
    {
        polygon *Poly = ClipSet.Polygons + I;
        Poly->Count = numVertices;
        Poly->Points = GenerateRandomPolygon(numVertices, -MaxX, MaxX, -MaxY, MaxY);
    }
    
    WritePolygonsToJSON(&SubjectSet, &ClipSet, "d:/Clipper-2d/output/polygons.json");

    for(s32 I = 0;
        I < PolygonCount;
        ++I)
    {
        polygon *S = SubjectSet.Polygons + I;
        polygon *C = ClipSet.Polygons + I;
        // Print the polygon vertices

        printf("==============================\n");
        printf("Polygon Pair[%d] Is: \n", I);

        printf("Subject[%d]: ", S->Count);
        for(int i = 0; i < numVertices; i++)
        {
            printf("(%f, %f), ", S->Points[i].x, S->Points[i].y);
        }

        printf("\n\n");

        printf("Clip[%d]: ", C->Count);
        for(int i = 0; i < numVertices; i++)
        {
            printf("(%f, %f), ", C->Points[i].x, C->Points[i].y);
        }

        free(S->Points);
        free(C->Points);
        printf("\n==============================\n");

        printf("\n\n");
    }
    
    // Free the allocated memory
    free(SubjectSet.Polygons);
    free(ClipSet.Polygons);
#endif
    
    EndAndPrintProfile();
    
    return(0);
}
