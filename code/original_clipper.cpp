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

    Assert((S->Count > 0) && (C->Count > 0));

    PathsD subject, clip, solution;

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

    solution = Difference(subject, clip, FillRule::EvenOdd);
#if 0
    solution = Intersect(subject, clip, FillRule::EvenOdd);

    solution = Union(subject, clip, FillRule::EvenOdd);

    solution = Xor(subject, clip, FillRule::EvenOdd);
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
