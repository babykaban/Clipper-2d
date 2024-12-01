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
GenerateRandomPolygonSIMD(s32 VertexCount, f32 MinX, f32 MaxX, f32 MinY, f32 MaxY)
{
    TimeFunction;

    Assert((VertexCount % 8) == 0)
    
    // Allocate memory for the polygon vertices
    f32 VertexCountInv = 1.0f / (f32)VertexCount;

    f32 *polygon_x;
    f32 *polygon_y;

    {
#if TIME_GENERATE
        TimeBlock("Allocate memory");
#endif
        polygon_x = (f32 *)malloc(VertexCount * sizeof(f32));
        polygon_y = (f32 *)malloc(VertexCount * sizeof(f32));
    }

    if((polygon_x == 0) || (polygon_y == 0))
    {
        return 0; // Memory allocation failed
    }

    u32 Increment = 8;
    u32 CountByEight = VertexCount / 8;
    
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

        f32_8x TmpCenter_x = Set1(0);
        f32_8x TmpCenter_y = Set1(0);
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
                v2_f32 P = V2F64(*(polygon_x + (VertexCount - 1)),
                                 *(polygon_y + (VertexCount - 1)));

                v2_f32 CP = P - Center;
                f64 angle = atan2(CP.y, CP.x);
                *p_x = angle;
            }
        }
    }

    v2_f32p *sort_array = (v2_f32p *)malloc(VertexCount*sizeof(v2_f32p));

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
        qsort(sort_array, VertexCount, sizeof(v2_f32), ComparePointsP);
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
    {
#if TIME_GENERATE
        TimeBlock("Print Poly");
#endif
        printf("poly0: ");
        for(s32 I = 0; I < VertexCount; ++I)
        {
            printf("(%.2f, %.2f), ", polygon_x[I], polygon_y[I]);
        }
        printf("\n\n");
    }    
#endif
    
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
