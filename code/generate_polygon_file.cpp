/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */
#include "main.h"
#include <time.h>

#include "clipper_memory.h"
#include "clipper_math.h"
#include "clipper_math_f64.h"

#define PROFILER 1
#include "profiler.cpp"
#include "generate_polygon_file.h"

#define TIME_GENERATE 0
#define TIME_PRINTS 1
#define PRINT 0

// NOTE(babykaban): TRIANGLES =====================================================

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
// ================================================================================

// NOTE(babykaban): POLYGONS ======================================================
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


// Function to calculate the orientation of three points (p, q, r)
// Returns 0 if collinear, 1 if clockwise, 2 if counterclockwise
inline s32
Orientation(v2_f32 p, v2_f32 q, v2_f32 r)
{
    f32 val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (val == 0)
        return 0;

    return((val > 0) ? 1 : 2);
}

// Function to check if point r lies on line segment pq
inline s32
OnSegment(v2_f32 p, v2_f32 r, v2_f32 q)
{
    if(r.x <= Maximum(p.x, q.x) && r.x >= Minimum(p.x, q.x) &&
       r.y <= Maximum(p.y, q.y) && r.y >= Minimum(p.y, q.y))
        return 1;

    return 0;
}

// Function to check if line segments p1q1 and p2q2 intersect
inline s32
DoIntersect(v2_f32 p1, v2_f32 q1, v2_f32 p2, v2_f32 q2)
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
    v2_f32 *p1 = (v2_f32 *)a;
    v2_f32 *p2 = (v2_f32 *)b;

    if (p1->x < p2->x) return -1;
    if (p1->x > p2->x) return 1;
    return 0; 
}

// Function to generate a random simple polygon with a given number of vertices
// within a bounding box
internal v2_f32 *
GenerateRandomPolygonF32(s32 VertexCount, f32 MinX, f32 MaxX, f32 MinY, f32 MaxY,
                         u32 Index = 0)
{
    // Allocate memory for the polygon vertices
    f32 VertexCountInv = 1.0f / (f32)VertexCount;

    v2_f32* polygon = (v2_f32*)malloc(VertexCount * sizeof(v2_f32));
    if(polygon == 0)
    {
        return 0; // Memory allocation failed
    }

    {
#if TIME_GENERATE
        TimeBlock("Generating points");
#endif
        // Generate random points
        for(s32 i = 0; i < VertexCount; i++)
        {
            polygon[i] = RandomPoint(MinX, MaxX, MinY, MaxY);
        }
    }

    v2_f32 center = {};
    {
#if TIME_GENERATE
        TimeBlock("Calculate Centers");
#endif

        // Sort points in counter-clockwise order using a simple wrapping algorithm
        // This helps avoid self-intersections
        for(s32 i = 0; i < VertexCount; i++)
        {
            center += polygon[i];
        }

        center *= VertexCountInv;
    }

    {
#if TIME_GENERATE
        TimeBlock("Compute angles");
#endif
    
        // Calculate angle for each point relative to the center
        for(s32 i = 0; i < VertexCount; i++)
        {
            v2_f32 CP = polygon[i] - center;
            f32 angle = atan2f(CP.y, CP.x);
            polygon[i].x = angle; // Temporarily store angle in x for sorting
        }
    }

    {
#if TIME_GENERATE
        TimeBlock("Sort");
#endif
        qsort(polygon, VertexCount, sizeof(v2_f32), ComparePoints);
    }

    {
#if TIME_GENERATE
        TimeBlock("Restore original x values");
#endif
        // Restore original x values
        for(s32 i = 0; i < VertexCount; i++)
        {
            f32 angle = polygon[i].x;
            polygon[i].x = center.x + cosf(angle) * (rand() / (f32)RAND_MAX) *
                (MaxX - MinX) / 2;
        }
    }
    
#if 1

    // Check for self-intersections and regenerate if found
    
    b32 hasIntersection = 0;
    {
        TimeBlock("CheckIntersect");
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
    }
    
    if(hasIntersection)
    {
        // Free the current polygon and try generating a new one
        free(polygon);

        return GenerateRandomPolygonF32(VertexCount, MinX, MaxX, MinY, MaxY, Index);
    }
#endif

#if PRINT
    {
#if TIME_PRINTS
        TimeBlock("Print Poly 2");
#endif
        PrintPoly(VertexCount, polygon, 2);
    }
#endif

    printf("Polygon %d Generated\n", Index);
    
    return polygon;
}

// ================================================================================

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
            printf("JSON: Poly pair %d written\n", I);
        }

        JSONWritePolyPair(Out, Ss->Polygons + (Ss->PolyCount - 1),
                          Cs->Polygons + (Cs->PolyCount - 1), Ss->PolyCount - 1, true);
        printf("JSON: Poly pair %d written\n", Ss->PolyCount - 1);
        
        fprintf(Out, "    ]\n");
        fprintf(Out, "}\n");
    }

    fclose(Out);
}

internal void
WritePolygonsToBIN(polygon_set *Ss, polygon_set *Cs, char *FileName)
{
    FILE *Out;

    u32 SubjectsIdentifier = 0xFFFF0000;
    u32 ClipsIdentifier = 0x0000FFFF;
    
    fopen_s(&Out, FileName, "wb");
    if(Out)
    {
        fwrite(&SubjectsIdentifier, sizeof(u32), 1, Out);
        fwrite(&Ss->PolyCount, sizeof(u32), 1, Out);
        for(u32 I = 0; I < Ss->PolyCount; ++I)
        {
            polygon *Poly = Ss->Polygons + I;
            fwrite(&Poly->Count, sizeof(u32), 1, Out);
            fwrite(Poly->Points, sizeof(v2_f32)*Poly->Count, 1, Out);

            printf("BIN S: Poly %d\n", I);
        }

        fwrite(&ClipsIdentifier, sizeof(u32), 1, Out);
        fwrite(&Cs->PolyCount, sizeof(u32), 1, Out);
        for(u32 I = 0; I < Cs->PolyCount; ++I)
        {
            polygon *Poly = Cs->Polygons + I;
            fwrite(&Poly->Count, sizeof(u32), 1, Out);
            fwrite(Poly->Points, sizeof(v2_f32)*Poly->Count, 1, Out);

            printf("BIN C: Poly %d\n", I);
        }
    }

    fclose(Out);
    
}

int
main()
{
    BeginProfile();

    srand((u32)time(0)); // Seed for randomness

    f32 MaxYf32 = 2000;
    f32 MaxXf32 = 2000;

    s32 PolygonCount = 262144*2;
    s32 numVertices = 16;

    polygon_set SubjectSet = {};
    SubjectSet.PolyCount = PolygonCount;
    SubjectSet.Polygons = (polygon *)malloc(sizeof(polygon)*PolygonCount);
    
    u32 Count = 0;
    for(s32 I = 0;
        I < PolygonCount;
        ++I)
    {
        polygon *Poly = SubjectSet.Polygons + I;
        Poly->Count = rand() % (16 - 3 + 1)  + 3;
        Poly->Points = GenerateRandomPolygonF32(Poly->Count, -MaxXf32, MaxXf32, -MaxYf32, MaxYf32, Count++);
    }

    polygon_set ClipSet = {};
    ClipSet.PolyCount = PolygonCount;
    ClipSet.Polygons = (polygon *)malloc(sizeof(polygon)*PolygonCount);

    for(s32 I = 0;
        I < PolygonCount;
        ++I)
    {
        polygon *Poly = ClipSet.Polygons + I;
        Poly->Count = rand() % (16 - 3 + 1)  + 3;
        Poly->Points = GenerateRandomPolygonF32(Poly->Count, -MaxXf32, MaxXf32, -MaxYf32, MaxYf32, Count++);
    }
    
    WritePolygonsToJSON(&SubjectSet, &ClipSet, "c:/Paul/Clipper-2d/output/polygons.json");
    WritePolygonsToBIN(&SubjectSet, &ClipSet, "c:/Paul/Clipper-2d/output/polygons_b.bin");

//    WritePolygonsToJSON(&SubjectSet, &ClipSet, "d:/Clipper-2d/output/polygons.json");
//    WritePolygonsToBIN(&SubjectSet, &ClipSet, "d:/Clipper-2d/output/polygons_b.bin");

#if PRINT
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
#endif
    
    // Free the allocated memory
    free(SubjectSet.Polygons);
    free(ClipSet.Polygons);
    
    EndAndPrintProfile();
    
    return(0);
}
