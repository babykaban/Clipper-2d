/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */
#include "generate_random_polyf32.h"

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

#if 1
internal v2_f32 *
GenerateRandomPolygonSIMDF32(s32 VertexCount, f32 MinX, f32 MaxX, f32 MinY, f32 MaxY)
{
    TimeFunction;

//    Assert((VertexCount % 8) == 0)
    
    // Allocate memory for the polygon vertices
    f32 VertexCountInv = 1.0f / (f32)VertexCount;

    f32 *polygon_x;
    f32 *polygon_y;

    s32 *Indecies;
    s32 *Temp;

    {
#if TIME_GENERATE
        TimeBlock("Allocate memory");
#endif
        polygon_x = (f32 *)malloc(VertexCount * sizeof(f32));
        polygon_y = (f32 *)malloc(VertexCount * sizeof(f32));
        Indecies = (s32 *)malloc(VertexCount * sizeof(s32));
        Temp = (s32 *)malloc(VertexCount * sizeof(s32));
    }

    if((polygon_x == 0) || (polygon_y == 0))
    {
        return 0; // Memory allocation failed
    }
    
    u32 Increment8x = 8;
    u32 Increment4x = 4;

    u32 Left = VertexCount % 8;
    u32 CountByEight = (VertexCount - Left) / 8;
    u32 CountByFour = (Left > 3) ? 1 : 0;
    u32 CountLeft = Left % 4;

    s32_8x Value = Set(7, 6, 5, 4, 3, 2, 1, 0);
    s32_8x One_8x = Set1(1);
    s32 *Itter = Indecies + 0;
    for(u32 I = 0; I < CountByEight; ++I)
    {
        StoreWI8(Itter, Value);
        Itter += Increment8x;
        Value = AddWI8(Value, One_8x);
    }

    if(Left)
    {
        s32 Before = VertexCount - Left;
        for(u32 I = 0; I < Left; ++I)
        {
            *Itter = Before;

            ++Before;
            ++Itter;
        }
    }
    
    {
#if TIME_GENERATE
        TimeBlock("Generating points");
#endif

        f32 *p_x = polygon_x + 0;
        f32 *p_y = polygon_y + 0;
        for(u32 I = 0;
            I < CountByEight;
            I++)
        {
            f32_8x Rand8_x0 = RandFloat_8x(MinX, MaxX);
            f32_8x Rand8_x1 = RandFloat_8x(MinY, MaxY);

            StoreWF8(p_x, Rand8_x0);
            StoreWF8(p_y, Rand8_x1);

            p_x += Increment8x;
            p_y += Increment8x;
        }

        if(CountByFour)
        {
            f32_4x Rand4_x0 = RandFloat_4x(MinX, MaxX);
            f32_4x Rand4_x1 = RandFloat_4x(MinY, MaxY);

            StoreWF4(p_x, Rand4_x0);
            StoreWF4(p_y, Rand4_x1);

            p_x += Increment4x;
            p_y += Increment4x;
        }

        if(CountLeft)
        {
            for(u32 I = 0; I < CountLeft; ++I)
            {
                *p_x = RandFloat(MinX, MaxX);
                *p_y = RandFloat(MinY, MaxY);

                ++p_x;
                ++p_y;
            }
        }
    }
    
    v2_f32 Center = {};
    f32_4x CenterW = Set1WF4(0.0f);
    {
#if TIME_GENERATE
        TimeBlock("Calculate Center");
#endif

        f32 *p_x = polygon_x + 0;
        f32 *p_y = polygon_y + 0;

        f32 X[8] = {};
        f32 Y[8] = {};

        if(CountByEight)
        {
            f32_8x TmpCenter_x = Set1WF8(0.0f);
            f32_8x TmpCenter_y = Set1WF8(0.0f);
            for(u32 I = 0; I < CountByEight; ++I)
            {
                f32_8x X_8x = LoadWF8(p_x);
                f32_8x Y_8x = LoadWF8(p_y);

                TmpCenter_x = AddWF8(TmpCenter_x, X_8x);
                TmpCenter_y = AddWF8(TmpCenter_y, Y_8x);

                p_x += Increment8x;
                p_y += Increment8x;
            }        

            StoreWF8(X, TmpCenter_x);
            StoreWF8(Y, TmpCenter_y);
        }

        f32_4x TmpCenter_x = Set1WF4(0);
        f32_4x TmpCenter_y = Set1WF4(0);
        f32_4x X_2x = Set1WF4(0);
        f32_4x Y_2x = Set1WF4(0);

        f32 *x = X + 0;
        f32 *y = Y + 0;
        f32_4x VertexCountInv_4x = Set1WF4(1.0f / (f32)VertexCount);

        u32 LoopCount = (CountByEight > 0) ? (CountByFour + 2) : CountByFour; 
        for(u32 I = 0; I < LoopCount; ++I)
        {
            if(I < CountByFour)
            {
                X_2x = LoadWF4(p_x);
                Y_2x = LoadWF4(p_y);

                p_x += Increment4x;
                p_y += Increment4x;
            }
            else
            {
                X_2x = LoadWF4(x);
                Y_2x = LoadWF4(y);
                x += Increment4x;
                y += Increment4x;
            }

            TmpCenter_x = AddWF4(TmpCenter_x, X_2x);
            TmpCenter_y = AddWF4(TmpCenter_y, Y_2x);
        }

        // NOTE(babykaban): Inside CenterW: [x, x, y, y]
        CenterW = HaddWF4(TmpCenter_x, TmpCenter_y);        

        f32 PreResult[4] = {};
        if(CountLeft)
        {
            f32 LeftX[4] = {};
            f32 LeftY[4] = {};
            for(u32 I = 0; I < CountLeft; ++I)
            {
                LeftX[I] = *p_x;
                LeftY[I] = *p_y;

                ++p_x;
                ++p_y;
            }

            f32_4x XLeft = LoadWF4(LeftX);
            f32_4x YLeft = LoadWF4(LeftY);

            // NOTE(babykaban): Inside SumLeft: [x, x, y, y]
            f32_4x SumLeft = HaddWF4(XLeft, YLeft);
            
            // NOTE(babykaban): Inside CenterW: [x, y, x, y]
            f32_4x Result = HaddWF4(CenterW, SumLeft);        
            StoreWF4(PreResult, Result);
        }

        Center =
            {
                (PreResult[0] + PreResult[2]) * VertexCountInv,
                (PreResult[1] + PreResult[3]) * VertexCountInv
            };
    }

    f32_8x CenterX_8x = Set1WF8(Center.x);
    f32_8x CenterY_8x = Set1WF8(Center.y);
    f32_4x CenterX_4x = Set1WF4(Center.x);
    f32_4x CenterY_4x = Set1WF4(Center.y);

    {
#if TIME_GENERATE
        TimeBlock("Compute angles");
#endif

        f32 *p_x = polygon_x + 0;
        f32 *p_y = polygon_y + 0;

        for(u32 I = 0; I < CountByEight; ++I)
        {
            f32_8x X_8x = LoadWF8(p_x);
            f32_8x Y_8x = LoadWF8(p_y);

            f32_8x CPX_8x = SubWF8(X_8x, CenterX_8x);
            f32_8x CPY_8x = SubWF8(Y_8x, CenterY_8x);

            f32_8x angle_8x = Atan2WF8(CPY_8x, CPX_8x);
            StoreWF8(p_x, angle_8x);
                
            p_x += Increment8x;
            p_y += Increment8x;
        }

        if(CountByFour)
        {
            f32_4x X_4x = LoadWF4(p_x);
            f32_4x Y_4x = LoadWF4(p_y);

            f32_4x CPX_4x = SubWF4(X_4x, CenterX_4x);
            f32_4x CPY_4x = SubWF4(Y_4x, CenterY_4x);

            f32_4x angle_4x = Atan2WF4(CPY_4x, CPX_4x);
            StoreWF4(p_x, angle_4x);
                
            p_x += Increment4x;
            p_y += Increment4x;
        }

        if(CountLeft)
        {
            for(u32 I = 0; I < CountLeft; ++I)
            {
                v2_f32 P = {*p_x, *p_y};
                v2_f32 CP = {(P.x - Center.x), (P.y - Center.y)};

                f32 angle = atan2f(CP.y, CP.x);
                *p_x = angle;

                ++p_x;
            }
        }
    }

    {
#if TIME_GENERATE
        TimeBlock("Sort");
#endif
        RadixSort(VertexCount, Indecies, Temp, polygon_x);
    }
    
    {
#if TIME_GENERATE
        TimeBlock("Restore original x values");
#endif

        f32 *p_x = polygon_x + 0;

        f32 HalfD = 0.5f*(MaxX - MinX);
        f32_8x HalfD_8x = Set1WF8(HalfD);
        f32_4x HalfD_4x = Set1WF4(HalfD);

        for(u32 I = 0; I < CountByEight; ++I)
        {
            f32_8x angle_8x = LoadWF8(p_x);
            f32_8x cos_8x = CosWF8(angle_8x);
            f32_8x rand_8x = RandFloat1_8x();

            f32_8x PreResult = MulWF8(MulWF8(cos_8x, rand_8x), HalfD_8x);
            f32_8x Result = AddWF8(CenterX_8x, PreResult);
                                               
            StoreWF8(p_x, Result);
            
            p_x += Increment8x;
        }

        if(CountByFour)
        {
            f32_4x angle_4x = LoadWF4(p_x);
            f32_4x cos_4x = CosWF4(angle_4x);
            f32_4x rand_4x = RandFloat1_4x();

            f32_4x PreResult = MulWF4(MulWF4(cos_4x, rand_4x), HalfD_4x);
            f32_4x Result = AddWF4(CenterX_4x, PreResult);
                                               
            StoreWF4(p_x, Result);
            
            p_x += Increment4x;
        }

        if(CountLeft)
        {
            for(u32 I = 0; I < CountLeft; ++I)
            {
                f32 angle = *p_x;;
                f32 cos = cosf(angle);

                f32 rand = RandFloat1();
                f32 PreResult = cos*rand*HalfD;
                f32 Result = Center.x + PreResult;

                *p_x = Result;
                ++p_x;
            }
        }
    }

    v2_f32 *polygon = (v2_f32 *)malloc(sizeof(v2_f32)*VertexCount);

    for(s32 I = 0; I < VertexCount; ++I)
    {
        polygon[I].x = polygon_x[Indecies[I]];
        polygon[I].y = polygon_y[Indecies[I]];
    }

#if 1    

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
        free(polygon_x);
        free(polygon_y);
        free(Indecies);
        free(Temp);

        return GenerateRandomPolygonSIMDF32(VertexCount, MinX, MaxX, MinY, MaxY);
    }
#endif


#if PRINT
    {
#if TIME_PRINTS
        TimeBlock("Print Poly 3");
#endif
        PrintPoly(VertexCount, polygon_x, polygon_y, Indecies, 3);
    }
#endif

    free(polygon_x);
    free(polygon_y);
    free(Indecies);
    free(Temp);

    return polygon;
}
#endif


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
GenerateRandomPolygonF32(s32 VertexCount, f32 MinX, f32 MaxX, f32 MinY, f32 MaxY)
{
    TimeFunction;

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

        return GenerateRandomPolygonF32(VertexCount, MinX, MaxX, MinY, MaxY);
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
    
    return polygon;
}
