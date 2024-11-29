/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */
#include "generate_random_polyf32.h"

internal v2_f32 *
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
                v2_f32 P = RandomPoint(MinX, MaxX, MinY, MaxY);
                *p_x = P.x;
                *p_y = P.y;
            }
        }
    }
    
    v2_f32 Center = {};
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
