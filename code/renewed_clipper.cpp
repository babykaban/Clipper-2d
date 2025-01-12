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

inline test_result
TESTTwoPolies(polygon *S, polygon *C, u32 Index = 0)
{
    TimeFunction;

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

    {
        TimeBlock("Difference");
        BooleanOpD(ClipType_Difference, FillRule_EvenOdd, &Subject, 0, &Clip,
                   &Solution, 0, true, false);
    }

    Result.Dif.PolyCount = Solution.PathCount;
    Result.Dif.Polygons = (polygon *)malloc(Result.Dif.PolyCount*sizeof(polygon));
    
    for(s32 PolyIndex = 0;
        PolyIndex < Solution.PathCount;
        ++PolyIndex)
    {
        path_f64 *Path = Solution.Paths + PolyIndex;

        polygon *Poly = Result.Dif.Polygons + PolyIndex;
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

    FreePaths(&Solution);
    
#if 1
    {
        TimeBlock("Intersection");
        BooleanOpD(ClipType_Intersection, FillRule_EvenOdd, &Subject, 0, &Clip,
                   &Solution, 0, true, false);
//        FreePaths(&Solution);
    }

    Result.Inter.PolyCount = Solution.PathCount;
    Result.Inter.Polygons = (polygon *)malloc(Result.Inter.PolyCount*sizeof(polygon));
    
    for(s32 PolyIndex = 0;
        PolyIndex < Solution.PathCount;
        ++PolyIndex)
    {
        path_f64 *Path = Solution.Paths + PolyIndex;

        polygon *Poly = Result.Inter.Polygons + PolyIndex;
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

    FreePaths(&Solution);
    
    {
        TimeBlock("Union");
        BooleanOpD(ClipType_Union, FillRule_EvenOdd, &Subject, 0, &Clip,
                   &Solution, 0, true, false);
//        FreePaths(&Solution);
    }

    Result.Union.PolyCount = Solution.PathCount;
    Result.Union.Polygons = (polygon *)malloc(Result.Union.PolyCount*sizeof(polygon));
    
    for(s32 PolyIndex = 0;
        PolyIndex < Solution.PathCount;
        ++PolyIndex)
    {
        path_f64 *Path = Solution.Paths + PolyIndex;

        polygon *Poly = Result.Union.Polygons + PolyIndex;
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

    FreePaths(&Solution);
    
    {
        TimeBlock("Xor");
        BooleanOpD(ClipType_Xor, FillRule_EvenOdd, &Subject, 0, &Clip,
                   &Solution, 0, true, false);
//        FreePaths(&Solution);
    }

    Result.Xor.PolyCount = Solution.PathCount;
    Result.Xor.Polygons = (polygon *)malloc(Result.Xor.PolyCount*sizeof(polygon));
    
    for(s32 PolyIndex = 0;
        PolyIndex < Solution.PathCount;
        ++PolyIndex)
    {
        path_f64 *Path = Solution.Paths + PolyIndex;

        polygon *Poly = Result.Xor.Polygons + PolyIndex;
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

    FreePaths(&Solution);

#endif

#if 0
    printf("Diff\n");
    for(u32 polyi = 0;
        polyi < Result.Dif.PolyCount;
        ++polyi)
    {
        polygon *Poly = Result.Dif.Polygons + polyi;
        printf("%d. ", polyi);
        for(u32 pi = 0;
            pi < Poly->Count;
            ++pi)
        {
            printf("(%.4f, %.4f), ", Poly->Points[pi].x, Poly->Points[pi].y);
        }

        printf("\n");
    }
    printf("\n");

    printf("Inter\n");
    for(u32 polyi = 0;
        polyi < Result.Inter.PolyCount;
        ++polyi)
    {
        polygon *Poly = Result.Inter.Polygons + polyi;
        printf("%d. ", polyi);
        for(u32 pi = 0;
            pi < Poly->Count;
            ++pi)
        {
            printf("(%.4f, %.4f), ", Poly->Points[pi].x, Poly->Points[pi].y);
        }

        printf("\n");
    }
    printf("\n");

    printf("Union\n");
    for(u32 polyi = 0;
        polyi < Result.Union.PolyCount;
        ++polyi)
    {
        polygon *Poly = Result.Union.Polygons + polyi;
        printf("%d. ", polyi);
        for(u32 pi = 0;
            pi < Poly->Count;
            ++pi)
        {
            printf("(%.4f, %.4f), ", Poly->Points[pi].x, Poly->Points[pi].y);
        }

        printf("\n");
    }
    printf("\n");

    printf("Xor\n");
    for(u32 polyi = 0;
        polyi < Result.Xor.PolyCount;
        ++polyi)
    {
        polygon *Poly = Result.Xor.Polygons + polyi;
        printf("%d. ", polyi);
        for(u32 pi = 0;
            pi < Poly->Count;
            ++pi)
        {
            printf("(%.4f, %.4f), ", Poly->Points[pi].x, Poly->Points[pi].y);
        }

        printf("\n");
    }
    printf("\n");

#endif    
    
#if PRINT_OUT_RESULT
    printf("\nTest[%d]: %s, %s", Index, ClipTypes[ClipType_Intersection], FillRules[FillRule_EvenOdd]);

    for(s32 PC = 0;
        PC < Solution.PathCount;
        ++PC)
    {
        printf("\n%d. ", PC);
        path_f64 *Path = Solution.Paths + PC;
        for(s32 J = 0;
            J < (Path->Count - 1);
            ++J)
        {
            printf("(%.2f, %.2f), ", Path->Points[J].x, Path->Points[J].y);
        }

        printf("(%.2f, %.2f)", Path->Points[Path->Count - 1].x, Path->Points[Path->Count - 1].y);
    }

    printf("\n");
#endif

    FreePaths(&Subject);
    FreePaths(&Clip);

    return(Result);
}
