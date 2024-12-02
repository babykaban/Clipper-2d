/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */
#include "generate_random_polyf32.h"

#if 1
internal v2_f32 *
GenerateRandomPolygonSIMDF32(s32 VertexCount, f32 MinX, f32 MaxX, f32 MinY, f32 MaxY)
{
    TimeFunction;

    Assert((VertexCount % 8) == 0)
    
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
    
    u32 Increment = 8;
    u32 CountByEight = VertexCount / 8;

    s32_8x Value = Set(0, 1, 2, 3, 4, 5, 6, 7);
    s32_8x One_8x = Set1(1);
    s32 *Itter = Indecies + 0;
    for(u32 I = 0; I < CountByEight; ++I)
    {
        StoreWI8(Itter, Value);
        Itter += Increment;
        Value = AddWI8(Value, One_8x);
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

            p_x += Increment;
            p_y += Increment;
        }
    }
    
    v2_f32 Center = {};
    {
#if TIME_GENERATE
        TimeBlock("Calculate Center");
#endif

        f32 *p_x = polygon_x + 0;
        f32 *p_y = polygon_y + 0;

        f32 X[8] = {};
        f32 Y[8] = {};

        f32_8x TmpCenter_x = Set1(0.0f);
        f32_8x TmpCenter_y = Set1(0.0f);
        for(u32 I = 0; I < CountByEight; ++I)
        {
            f32_8x X_8x = LoadWF8(p_x);
            f32_8x Y_8x = LoadWF8(p_y);

            TmpCenter_x = AddWF8(TmpCenter_x, X_8x);
            TmpCenter_y = AddWF8(TmpCenter_y, Y_8x);

            p_x += Increment;
            p_y += Increment;
        }        

        StoreWF8(X, TmpCenter_x);
        StoreWF8(Y, TmpCenter_y);

        for(u32 I = 0; I < Increment; ++I)
        {
            Center.x += X[I];
            Center.y += Y[I];
        }
        
        f32 VertexCountInv = 1.0f / (f32)VertexCount;
        Center *= VertexCountInv;
    }

    f32_8x CenterX_8x = Set1(Center.x);
    f32_8x CenterY_8x = Set1(Center.y);

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
                
            p_x += Increment;
            p_y += Increment;
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
        f32_8x HalfD_8x = Set1(HalfD);

        for(u32 I = 0; I < CountByEight; ++I)
        {
            f32_8x angle_8x = LoadWF8(p_x);
            f32_8x cos_8x = CosWF8(angle_8x);
            f32_8x rand_8x = RandFloat1_8x();

            f32_8x PreResult = MulWF8(MulWF8(cos_8x, rand_8x), HalfD_8x);
            f32_8x Result = AddWF8(CenterX_8x, PreResult);
                                               
            StoreWF8(p_x, Result);
            
            p_x += Increment;
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
        TimeBlock("Print Poly 3");
#endif
        PrintPoly(VertexCount, polygon_x, polygon_y, Indecies, 3);
    }
#endif

//    return polygon;
    return 0;
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
        TimeBlock("Print Poly 2");
#endif
        PrintPoly(VertexCount, polygon, 2);
    }
#endif
    
    return polygon;
}
