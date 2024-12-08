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

#include "clipper_simd.h"

#define PROFILER 1
#include "profiler.cpp"
#include "pcg_random.cpp"

#include "generate_polygon_file.h"

#include "clipper_memory.h"

#define TIME_GENERATE 0
#define TIME_PRINTS 1
#define PRINT 0

#include "generate_random_polyf32.cpp"

internal void
MergeSort(u32 Count, u32 *First, f64 *Angles)
{
    u32 *Temp = (u32 *)malloc(sizeof(u32)*Count);

    if(Count == 1)
    {
        // NOTE(casey): No work to do.
    }
    else if(Count == 2)
    {
        u32 *EntryA = First;
        u32 *EntryB = First + 1;
        if(AngleSorter(Angles[*EntryA], Angles[*EntryB]))
        {
            Swap(EntryA, EntryB);
        }
    }
    else
    {
        u32 Half0 = Count / 2;
        u32 Half1 = Count - Half0;

        Assert(Half0 >= 1);
        Assert(Half1 >= 1);

        u32 *InHalf0 = First;
        u32 *InHalf1 = First + Half0;
        u32 *End = First + Count;

        MergeSort(Half0, InHalf0, Angles);
        MergeSort(Half1, InHalf1, Angles);

        u32 *ReadHalf0 = InHalf0;
        u32 *ReadHalf1 = InHalf1;

        u32 *Out = Temp;
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            if(ReadHalf0 == InHalf1)
            {
                *Out++ = *ReadHalf1++;
            }
            else if(ReadHalf1 == End)
            {
                *Out++ = *ReadHalf0++;
            }
            else if(!AngleSorter(Angles[*ReadHalf0], Angles[*ReadHalf1]))
            {
                *Out++ = *ReadHalf0++;
            }
            else
            {
                *Out++ = *ReadHalf1++;
            }            
        }
        Assert(Out == (Temp + Count));
        Assert(ReadHalf0 == InHalf1);
        Assert(ReadHalf1 == End);
            
        Copy(sizeof(u32)*Count, Temp, First);
    }

    free(Temp);
}

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
ComparePointsF64(const void* a, const void* b)
{
    v2_f64 *p1 = (v2_f64 *)a;
    v2_f64 *p2 = (v2_f64 *)b;

    if (p1->x < p2->x) return -1;
    if (p1->x > p2->x) return 1;
    return 0; 
}

inline s32
ComparePointsP(const void* a, const void* b)
{
    v2_f64p *p1 = (v2_f64p *)a;
    v2_f64p *p2 = (v2_f64p *)b;

    if (*p1->x < *p2->x) return -1;
    if (*p1->x > *p2->x) return 1;
    return 0; 
}


// Comparison function for qsort to sort points by angle
inline s32
CompareValues(const void* a, const void* b)
{
    f64 *p1 = (f64 *)a;
    f64 *p2 = (f64 *)b;

    if (*p1 < *p2) return -1;
    if (*p1 > *p2) return 1;
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
#if TIME_GENERATE
        TimeBlock("Generating points");
#endif
        // Generate random points
        for(s32 i = 0; i < VertexCount; i++)
        {
            polygon[i] = RandomPoint(MinX, MaxX, MinY, MaxY);
        }
    }

    v2_f64 center = {};
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
            v2_f64 CP = polygon[i] - center;
            f64 angle = atan2(CP.y, CP.x);
            polygon[i].x = angle; // Temporarily store angle in x for sorting
        }
    }

    {
#if TIME_GENERATE
        TimeBlock("Sort");
#endif
        qsort(polygon, VertexCount, sizeof(v2_f64), ComparePointsF64);
    }

    {
#if TIME_GENERATE
        TimeBlock("Restore original x values");
#endif
        // Restore original x values
        for(s32 i = 0; i < VertexCount; i++)
        {
            f64 angle = polygon[i].x;
            polygon[i].x = center.x + cos(angle) * (rand() / (double)RAND_MAX) *
                (MaxX - MinX) / 2;
        }
    }
    
#if 0

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
//        free(polygon);

//        return GenerateRandomPolygon(VertexCount, MinX, MaxX, MinY, MaxY);
    }
#endif

#if PRINT
    {
#if TIME_PRINTS
        TimeBlock("Print Poly 0");
#endif
        PrintPoly(VertexCount, polygon, 0);
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

    f64 *polygon_x;
    f64 *polygon_y;

    {
#if TIME_GENERATE
        TimeBlock("Allocate memory");
#endif
        polygon_x = (f64 *)malloc(VertexCount * sizeof(f64));
        polygon_y = (f64 *)malloc(VertexCount * sizeof(f64));
    }

    if((polygon_x == 0) || (polygon_y == 0))
    {
        return 0; // Memory allocation failed
    }

    u32 Left = VertexCount % 4;
    u32 CountByFour = (VertexCount - Left) / 4;
    u32 CountByTwo = (Left > 1) ? 1 : 0;
    u32 CountLeft = Left % 2;
    
    {
#if TIME_GENERATE
        TimeBlock("Generating points");
#endif

        f64 *p_x = polygon_x + 0;
        f64 *p_y = polygon_y + 0;

        if(CountByFour)
        {
            for(u32 I = 0;
                I < CountByFour;
                I++)
            {
                __m256d Rand4_x0 = RandDouble_4x(MinX, MaxX);
                __m256d Rand4_x1 = RandDouble_4x(MinY, MaxY);
                _mm256_store_pd(p_x, Rand4_x0);
                _mm256_store_pd(p_y, Rand4_x1);
                p_x += 4;
                p_y += 4;
            }
        }

        if(CountByTwo)
        {
            __m128d Rand2_x0 = RandDouble_2x(MinX, MaxX);
            __m128d Rand2_x1 = RandDouble_2x(MinY, MaxY);
            _mm_store_pd(p_x, Rand2_x0);
            _mm_store_pd(p_y, Rand2_x1);

            p_x += 2;
            p_y += 2;

            if(CountLeft)
            {
                v2_f64 P = RandomPoint(MinX, MaxX, MinY, MaxY);
                *p_x = P.x;
                *p_y = P.y;
            }
        }
    }
    
    v2_f64 Center = {};
    {
#if TIME_GENERATE
        TimeBlock("Calculate Center");
#endif

        f64 *p_x = polygon_x + 0;
        f64 *p_y = polygon_y + 0;

        f64 X[4] = {};
        f64 Y[4] = {};

        if(CountByFour)
        {
            __m256d TmpCenter_x = _mm256_set1_pd(0);
            __m256d TmpCenter_y = _mm256_set1_pd(0);
            for(u32 I = 0; I < CountByFour; ++I)
            {
                __m256d X_4x = _mm256_load_pd(p_x);

                __m256d Y_4x = _mm256_load_pd(p_y);

                TmpCenter_x = _mm256_add_pd(TmpCenter_x, X_4x);
                TmpCenter_y = _mm256_add_pd(TmpCenter_y, Y_4x);

                p_x += 4;
                p_y += 4;
            }
        
            _mm256_store_pd(X, TmpCenter_x);
            _mm256_store_pd(Y, TmpCenter_y);
        }

        __m128d TmpCenter_x = _mm_set1_pd(0);
        __m128d TmpCenter_y = _mm_set1_pd(0);
        __m128d X_2x = _mm_set1_pd(0);
        __m128d Y_2x = _mm_set1_pd(0);
        f64 *x = X + 0;
        f64 *y = Y + 0;

        __m128d VertexCountInv_2x = _mm_set1_pd(1.0 / (f64)VertexCount);
        
        u32 LoopCount = (CountByFour > 0) ? (CountByTwo + 2) : CountByTwo; 
        for(u32 I = 0; I < LoopCount; ++I)
        {
            if(I < CountByTwo)
            {
                X_2x = _mm_load_pd(p_x);
                Y_2x = _mm_load_pd(p_y);

                p_x += 2;
                p_y += 2;
            }
            else
            {
                X_2x = _mm_load_pd(x);
                Y_2x = _mm_load_pd(y);
                x += 2;
                y += 2;
            }

            TmpCenter_x = _mm_add_pd(TmpCenter_x, X_2x);
            TmpCenter_y = _mm_add_pd(TmpCenter_y, Y_2x);
        }

        Center.W = _mm_hadd_pd(TmpCenter_x, TmpCenter_y);        

        if(CountLeft)
        {
            Center += V2F64(*(polygon_x + (VertexCount - 1)),
                            *(polygon_y + (VertexCount - 1)));
        }

        Center.W = _mm_mul_pd(Center.W, VertexCountInv_2x);
    }

    __m256d CenterX_4x = _mm256_set1_pd(Center.x);
    __m256d CenterY_4x = _mm256_set1_pd(Center.y);
    __m128d CenterX_2x = _mm_set1_pd(Center.x);
    __m128d CenterY_2x = _mm_set1_pd(Center.y);

    {
#if TIME_GENERATE
        TimeBlock("Compute angles");
#endif

        f64 *p_x = polygon_x + 0;
        f64 *p_y = polygon_y + 0;

        if(CountByFour)
        {
            for(u32 I = 0; I < CountByFour; ++I)
            {
                __m256d X_4x = _mm256_load_pd(p_x);
                __m256d Y_4x = _mm256_load_pd(p_y);

                __m256d CPX_4x = _mm256_sub_pd(X_4x, CenterX_4x);
                __m256d CPY_4x = _mm256_sub_pd(Y_4x, CenterY_4x);

                __m256d angle_4x = _mm256_atan2_pd(CPY_4x, CPX_4x);
                _mm256_store_pd(p_x, angle_4x);
                
                p_x += 4;
                p_y += 4;
            }
        }

        if(CountByTwo)
        {
            __m128d X_2x = _mm_load_pd(p_x);
            __m128d Y_2x = _mm_load_pd(p_y);

            __m128d CPX_2x = _mm_sub_pd(X_2x, CenterX_2x);
            __m128d CPY_2x = _mm_sub_pd(Y_2x, CenterY_2x);

            __m128d angle_2x = _mm_atan2_pd(CPY_2x, CPX_2x);
            _mm_store_pd(p_x, angle_2x);
                
            p_x += 2;
            p_y += 2;

            if(CountLeft)
            {
                v2_f64 P = V2F64(*(polygon_x + (VertexCount - 1)),
                                 *(polygon_y + (VertexCount - 1)));

                v2_f64 CP = P - Center;
                f64 angle = atan2(CP.y, CP.x);
                *p_x = angle;
            }
        }
    }

    v2_f64p *sort_array = (v2_f64p *)malloc(VertexCount*sizeof(v2_f64p));

    {

#if TIME_GENERATE
        TimeBlock("Fill Sort Array");
#endif

        f64 *p_x = polygon_x + 0;
        f64 *p_y = polygon_y + 0;

        for(s32 I = 0; I < VertexCount; ++I)
        {
            sort_array[I].x = p_x;
            sort_array[I].y = p_y;

            ++p_x;
            ++p_y;
        }
    }

    {
#if TIME_GENERATE
        TimeBlock("Sort");
#endif
        qsort(sort_array, VertexCount, sizeof(v2_f64p), ComparePointsP);
    }

    {
#if TIME_GENERATE
        TimeBlock("Read after sort");
#endif

#if 0
        f64 *p_x = polygon_x + 0;
        f64 *p_y = polygon_y + 0;

        for(s32 I = 0; I < VertexCount; I += 4)
        {
            v4_f64 X =
                {
                    sort_array[I + 0].x, sort_array[I + 1].x,
                    sort_array[I + 2].x, sort_array[I + 3].x
                };

            v4_f64 Y =
                {
                    sort_array[I + 0].y, sort_array[I + 1].y,
                    sort_array[I + 2].y, sort_array[I + 3].y
                };

            _mm256_store_pd(p_x, X.W);
            _mm256_store_pd(p_y, Y.W);

            p_x += 4;
            p_y += 4;
        }
#endif
        free(sort_array);
    }
    
    {
#if TIME_GENERATE
        TimeBlock("Restore original x values");
#endif

        f64 *p_x = polygon_x + 0;

        f64 HalfD = 0.5*(MaxX - MinX);
        __m256d HalfD_4x = _mm256_set1_pd(HalfD);
        __m128d HalfD_2x = _mm_set1_pd(HalfD);

        if(CountByFour)
        {
            for(u32 I = 0; I < CountByFour; ++I)
            {
                __m256d angle_4x = _mm256_load_pd(p_x);
                __m256d cos_4x = _mm256_cos_pd(angle_4x);
                __m256d rand_4x = RandDouble1_4x();

                __m256d PreResult = _mm256_mul_pd(_mm256_mul_pd(cos_4x, rand_4x), HalfD_4x);
                __m256d Result = _mm256_add_pd(CenterX_4x, PreResult);
                                               
                _mm256_store_pd(p_x, Result);
            
                p_x += 4;
            }
            
        }

        if(CountByTwo)
        {
            __m128d angle_2x = _mm_load_pd(p_x);
            __m128d cos_2x = _mm_cos_pd(angle_2x);
            __m128d rand_2x = RandDouble1_2x();

            __m128d PreResult = _mm_mul_pd(_mm_mul_pd(cos_2x, rand_2x), HalfD_2x);
            __m128d Result = _mm_add_pd(CenterX_2x, PreResult);
                                               
            _mm_store_pd(p_x, Result);
            
            p_x += 2;
            
            if(CountLeft)
            {
                f64 X = *(polygon_x + (VertexCount - 1));
                f64 Cos = cos(X) * RandDouble1() * HalfD;
                *p_x = Cos + Center.x;
            }
        }
    }
    
#if 0    

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

#if PRINT
    {
#if TIME_PRINTS
        TimeBlock("Print Poly 1");
#endif
        PrintPoly(VertexCount, polygon_x, polygon_y, 1);
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

    __m128 c = _mm_set_ps(1, 2, 3, 4);
    __m128 d = _mm_set_ps(5, 6, 7, 8);
    __m128 r0 = _mm_hadd_ps(c, d);
#if 0
    u32 Count = 100000;
    u32 *Indecies = (u32 *)malloc(sizeof(u32)*Count);
    u32 *Temp = (u32 *)malloc(sizeof(u32)*Count);
    f32 *X = (f32 *)malloc(sizeof(f32)*Count);
    f32 *Y = (f32 *)malloc(sizeof(f32)*Count);

    for(u32 I = 0;
        I < Count;
        ++I)
    {
        Indecies[I] = I;
        X[I] = (f32)RandDouble1();
        Y[I] = (f32)RandDouble1();
    }

    {
        TimeBlock("TEST");
        RadixSort(Count, Indecies, Temp, X);
    }
    //MergeSort(Count, Indecies, X);
#endif
    
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

    f64 MaxY = 100;
    f64 MaxX = 100;
    f32 MaxYf32 = 1000;
    f32 MaxXf32 = 1000;

    s32 PolygonCount = 32000;
//    s32 PolygonCount = 100;
//    s32 numVertices = 12;
//    s32 numVertices = 13;
//    s32 numVertices = 14;
    s32 numVertices = 12;

    polygon_set SubjectSet = {};
    SubjectSet.PolyCount = PolygonCount;
    SubjectSet.Polygons = (polygon *)malloc(sizeof(polygon)*PolygonCount);
#if 0    
    for(s32 I = 0; I < PolygonCount; ++I)
    {
        GenerateRandomPolygonF32(numVertices, -MaxXf32, MaxXf32, -MaxYf32, MaxYf32);
    }

    for(s32 I = 0; I < PolygonCount; ++I)
    {
        GenerateRandomPolygonSIMDF32(numVertices, -MaxXf32, MaxXf32, -MaxYf32, MaxYf32);
    }
#endif
    
#if 1
    for(s32 I = 0;
        I < PolygonCount;
        ++I)
    {
        polygon *Poly = SubjectSet.Polygons + I;
        Poly->Count = rand() % (16 - 3 + 1)  + 3;
        Poly->Points = GenerateRandomPolygonF32(Poly->Count, -MaxXf32, MaxXf32, -MaxYf32, MaxYf32);
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
        Poly->Points = GenerateRandomPolygonF32(Poly->Count, -MaxXf32, MaxXf32, -MaxYf32, MaxYf32);
    }
    
    WritePolygonsToJSON(&SubjectSet, &ClipSet, "c:/Paul/Clipper-2d/output/polygons.json");

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
