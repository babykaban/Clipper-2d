/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */

#define USINGZ 0


#include "clipper2.h"
#include "clipper.engine.cpp"

inline void
TESTTwoPoliesOriginal(polygon *S, polygon *C, u32 Index = 0)
{
    TimeFunction;
    using namespace Clipper2Lib;

    test_result Result = {};

    Assert((S->Count > 0) && (C->Count > 0));

    PathsD subject, clip;
    PathsD solution[4];

    PathD tmp0;
    for(u32 I = 0; I < S->Count; ++I)
    {
        tmp0.push_back({S->Points[I].x, S->Points[I].y});        
    }
    subject.push_back(tmp0);

    
    PathD tmp1;
    for(u32 I = 0; I < C->Count; ++I)
    {
        tmp1.push_back({C->Points[I].x, C->Points[I].y});        
    }
    clip.push_back(tmp1);

    {
        TimeBlock("Difference");
        solution[0] = Difference(subject, clip, FillRule::EvenOdd);
    }
#if 1
    {
        TimeBlock("Intersection");
        solution[1] = Intersect(subject, clip, FillRule::EvenOdd);
    }
    
    {
        TimeBlock("Union");
        solution[2] = Union(subject, clip, FillRule::EvenOdd);
    }

    {
        TimeBlock("Xor");
        solution[3] = Xor(subject, clip, FillRule::EvenOdd);
    }    
#endif
    
#if PRINT_OUT_RESULT
    printf("\nTest[%d]: %s, %s", Index, ClipTypes[1], FillRules[0]);

    for (const PathD& path : solution) {
        for (const PointD& pt : path) {
            std::cout << "(" << pt.x << ", " << pt.y << "),";
        }
        std::cout << std::endl;
    }

    printf("\n");
#endif

    
}
