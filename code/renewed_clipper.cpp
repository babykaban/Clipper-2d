/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */

#include "clipper.h"
#include "clipper_sort.cpp"
#include "clipper.cpp"

inline void
RecordResult(polygon_set *Dest, u32 Count, path_f64 *Paths)
{
    for(u32 PolyIndex = 0;
        PolyIndex < Count;
        ++PolyIndex)
    {
        path_f64 *Path = Paths + PolyIndex;

        polygon *Poly = Dest->Polygons + PolyIndex;
        Poly->Count = Path->Count;
        Poly->Points = (v2_f32 *)malloc(Poly->Count*sizeof(v2_f32));

        for(s32 PointIndex = 0;
            PointIndex < Path->Count;
            ++PointIndex)
        {
            Poly->Points[PointIndex] =
                {
                    (f32)Path->Points[PointIndex].x,
                    (f32)Path->Points[PointIndex].y
                };
        }
    }
}

inline test_result
TESTTwoPolies(polygon *S, polygon *C, u32 FillRule, u32 Index = 0)
{
//    TimeFunction;

    test_result Result = {};
    
    Assert((S->Count > 0) && (C->Count > 0));

    paths_f64 Subject = GetPathsF64(1);
    Subject.PathCount = 1;
    Subject.Paths[0] = GetPathF64(S->Count);
    Subject.Paths[0].Count = S->Count;
    for(u32 I = 0; I < S->Count; ++I)
    {
        Subject.Paths[0].Points[I] = V2F64(S->Points[I]);        
    }
    
    paths_f64 Clip = GetPathsF64(1);
    Clip.PathCount = 1;
    Clip.Paths[0] = GetPathF64(C->Count);
    Clip.Paths[0].Count = C->Count;

    for(u32 I = 0; I < C->Count; ++I)
    {
        Clip.Paths[0].Points[I] = V2F64(C->Points[I]);        
    }

    paths_f64 Solution = {};

#if TEST_DIFFERENCE
    {
        TimeBlock("New Difference");
        BooleanOpD(ClipType_Difference, FillRule, &Subject, 0, &Clip,
                   &Solution, 0, true, false);
    }

    Result.Dif.PolyCount = Solution.PathCount;
    Result.Dif.Polygons = (polygon *)malloc(Result.Dif.PolyCount*sizeof(polygon));

    RecordResult(&Result.Dif, Solution.PathCount, Solution.Paths);

    FreePaths(&Solution);
#endif
    
#if TEST_INTERSECT
    {
        TimeBlock("New Intersection");
        BooleanOpD(ClipType_Intersection, FillRule, &Subject, 0, &Clip,
                   &Solution, 0, true, false);
    }

    Result.Inter.PolyCount = Solution.PathCount;
    Result.Inter.Polygons = (polygon *)malloc(Result.Inter.PolyCount*sizeof(polygon));

    RecordResult(&Result.Inter, Solution.PathCount, Solution.Paths);

    FreePaths(&Solution);
#endif

#if TEST_UNION
    {
        TimeBlock("New Union");
        BooleanOpD(ClipType_Union, FillRule, &Subject, 0, &Clip,
                   &Solution, 0, true, false);
    }

    Result.Union.PolyCount = Solution.PathCount;
    Result.Union.Polygons = (polygon *)malloc(Result.Union.PolyCount*sizeof(polygon));

    RecordResult(&Result.Union, Solution.PathCount, Solution.Paths);

    FreePaths(&Solution);
#endif

#if TEST_XOR
    {
        TimeBlock("New Xor");
        BooleanOpD(ClipType_Xor, FillRule, &Subject, 0, &Clip,
                   &Solution, 0, true, false);
    }

    Result.Xor.PolyCount = Solution.PathCount;
    Result.Xor.Polygons = (polygon *)malloc(Result.Xor.PolyCount*sizeof(polygon));

    RecordResult(&Result.Xor, Solution.PathCount, Solution.Paths);

    FreePaths(&Solution);
#endif

#if 0
    printf("\nTest[%d]: %s, %s\n", Index,
           ClipTypes[ClipType_Intersection],
           FillRules[FillRule_EvenOdd]);
#endif
    
#if PRINT_DIFFERENCE
    printf("Original Difference\n");
    PrintPolygons(Result.Dif.PolyCount, Result.Dif.Polygons);
    printf("\n");
#endif

#if PRINT_INTERSECT
    printf("Original Intersect\n");
    PrintPolygons(Result.Inter.PolyCount, Result.Inter.Polygons);
    printf("\n");
#endif

#if PRINT_UNION
    printf("Original Union\n");
    PrintPolygons(Result.Union.PolyCount, Result.Union.Polygons);
    printf("\n");
#endif

#if PRINT_XOR
    printf("Original Xor\n");
    PrintPolygons(Result.Xor.PolyCount, Result.Xor.Polygons);
    printf("\n");
#endif    

    FreePaths(&Subject);
    FreePaths(&Clip);

    return(Result);
}
