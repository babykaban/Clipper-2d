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

inline test_result
TESTTwoPoliesOriginal(polygon *S, polygon *C, u32 Index = 0)
{
    TimeFunction;
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

    {
        TimeBlock("Difference");
        solution = Difference(subject, clip, FillRule::EvenOdd);
    }

    Result.Dif.PolyCount = (u32)solution.size();
    Result.Dif.Polygons = (polygon *)malloc(Result.Dif.PolyCount*sizeof(polygon));
    u32 PolyIndex = 0;
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
    
#if 1
    {
        TimeBlock("Intersection");
        solution = Intersect(subject, clip, FillRule::EvenOdd);
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
    
    {
        TimeBlock("Union");
        solution = Union(subject, clip, FillRule::EvenOdd);
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

    {
        TimeBlock("Xor");
        solution = Xor(subject, clip, FillRule::EvenOdd);
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

#if 1
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

    int a = 0;
    
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

    return(Result);
}
