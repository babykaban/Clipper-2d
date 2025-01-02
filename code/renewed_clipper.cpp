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
TESTTwoPolies(polygon *S, polygon *C, u32 Index = 0)
{
    TimeFunction;
    
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
        BooleanOpD(ClipType_Difference, FillRule_EvenOdd, &Subject, 0, &Clip, &Solution, 0, true, false);
        FreePaths(&Solution);
    }

#if 1
    {
        TimeBlock("Intersection");
        BooleanOpD(ClipType_Intersection, FillRule_EvenOdd, &Subject, 0, &Clip, &Solution, 0, true, false);
        FreePaths(&Solution);
    }
    
    {
        TimeBlock("Union");
        BooleanOpD(ClipType_Union, FillRule_EvenOdd, &Subject, 0, &Clip, &Solution, 0, true, false);
        FreePaths(&Solution);
    }
    
    {
        TimeBlock("Xor");
        BooleanOpD(ClipType_Xor, FillRule_EvenOdd, &Subject, 0, &Clip, &Solution, 0, true, false);
        FreePaths(&Solution);
    }
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
}
