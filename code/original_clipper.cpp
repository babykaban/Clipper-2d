/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */

//#define USINGZ 0


#include "clipper2.h"
#include "clipper.engine.cpp"

inline test_result
TESTTwoPoliesOriginal(polygon *S, polygon *C, u32 fillRule, u32 Index = 0)
{
//    TimeFunction;
    using namespace Clipper2Lib;

    test_result Result = {};

    Assert((S->Count > 0) && (C->Count > 0));

    PathsD subject, clip;
    PathsD solution;

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

    u32 PolyIndex = 0;
#if TEST_DIFFERENCE
    {
//        TimeBlock("Original Difference");
        solution = Difference(subject, clip, (FillRule)fillRule);
    }

    Result.Dif.PolyCount = (u32)solution.size();
    Result.Dif.Polygons = (polygon *)malloc(Result.Dif.PolyCount*sizeof(polygon));
    PolyIndex = 0;
    for(PathD path : solution)
    {
        polygon *Poly = Result.Dif.Polygons + PolyIndex;
        Poly->Count = (u32)path.size();
        Poly->Points = (v2_f32 *)malloc(Poly->Count*sizeof(v2_f32));

        u32 PointIndex = 0;
        for(PointD point : path)
        {
            Poly->Points[PointIndex] = {(f32)point.x, (f32)point.y};
            ++PointIndex;
        }

        ++PolyIndex;
    }
#endif
    
#if TEST_INTERSECT
    {
//        TimeBlock("Original Intersection");
        solution = Intersect(subject, clip, (FillRule)fillRule);
    }

    Result.Inter.PolyCount = (u32)solution.size();
    Result.Inter.Polygons = (polygon *)malloc(Result.Inter.PolyCount*sizeof(polygon));
    PolyIndex = 0;
    for(PathD path : solution)
    {
        polygon *Poly = Result.Inter.Polygons + PolyIndex;
        Poly->Count = (u32)path.size();
        Poly->Points = (v2_f32 *)malloc(Poly->Count*sizeof(v2_f32));

        u32 PointIndex = 0;
        for(PointD point : path)
        {
            Poly->Points[PointIndex] = {(f32)point.x, (f32)point.y};
            ++PointIndex;
        }

        ++PolyIndex;
    }
#endif
    
#if TEST_UNION
    {
//        TimeBlock("Original Union");
        solution = Union(subject, clip, (FillRule)fillRule);
    }

    Result.Union.PolyCount = (u32)solution.size();
    Result.Union.Polygons = (polygon *)malloc(Result.Union.PolyCount*sizeof(polygon));
    PolyIndex = 0;
    for(PathD path : solution)
    {
        polygon *Poly = Result.Union.Polygons + PolyIndex;
        Poly->Count = (u32)path.size();
        Poly->Points = (v2_f32 *)malloc(Poly->Count*sizeof(v2_f32));

        u32 PointIndex = 0;
        for(PointD point : path)
        {
            Poly->Points[PointIndex] = {(f32)point.x, (f32)point.y};
            ++PointIndex;
        }

        ++PolyIndex;
    }
#endif

#if TEST_XOR
    {
//        TimeBlock("Original Xor");
        solution = Xor(subject, clip, (FillRule)fillRule);
    }    

    Result.Xor.PolyCount = (u32)solution.size();
    Result.Xor.Polygons = (polygon *)malloc(Result.Xor.PolyCount*sizeof(polygon));
    PolyIndex = 0;
    for(PathD path : solution)
    {
        polygon *Poly = Result.Xor.Polygons + PolyIndex;
        Poly->Count = (u32)path.size();
        Poly->Points = (v2_f32 *)malloc(Poly->Count*sizeof(v2_f32));

        u32 PointIndex = 0;
        for(PointD point : path)
        {
            Poly->Points[PointIndex] = {(f32)point.x, (f32)point.y};
            ++PointIndex;
        }

        ++PolyIndex;
    }
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

    return(Result);
}
