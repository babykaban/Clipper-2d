/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "main.h"

#define PROFILER 1
#define PRINT_OUT_RESULT 0

#define USINGZ 0
#include "profiler.cpp"

#include "clipper2.h"

#include "clipper.engine.cpp"
//#include "clipper.offset.cpp"
//#include "clipper.rectclip.cpp"

struct v2_f32
{
    f32 x;
    f32 y;
};

struct polygon
{
    u32 Count;
    v2_f32 *Points;
};

struct polygon_set
{
    u32 PolyCount;
    polygon *Polygons;
};

inline void
IntersectTwoPolies(polygon *S, polygon *C, u32 Index = 0)
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

//    solution = Intersect(subject, clip, FillRule::EvenOdd);
    solution = Difference(subject, clip, FillRule::EvenOdd);
//    solution = Union(subject, clip, FillRule::EvenOdd);
//    solution = Xor(subject, clip, FillRule::EvenOdd);
    
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

internal void
ReadPolies(polygon_set *Subjects, polygon_set *Clips, char *FileName)
{
    TimeFunction;

    FILE *In;
    fopen_s(&In, FileName, "rb");
    if(In)
    {
        u32 SubjectsIdentifier = 0;
        fread(&SubjectsIdentifier, sizeof(u32), 1, In);
        Assert(SubjectsIdentifier == 0xFFFF0000);

        fread(&Subjects->PolyCount, sizeof(u32), 1, In);
        Assert(Subjects->PolyCount != 0);

        Subjects->Polygons = (polygon *)malloc(sizeof(polygon)*Subjects->PolyCount);
        for(u32 I = 0; I < Subjects->PolyCount; ++ I)
        {
            polygon *Poly = Subjects->Polygons + I;
            fread(&Poly->Count, sizeof(u32), 1, In);
            Assert(Poly->Count != 0);

            Poly->Points = (v2_f32 *)malloc(sizeof(v2_f32)*Poly->Count);
            
            fread(Poly->Points, sizeof(v2_f32)*Poly->Count, 1, In);
#if PRINT_READ
            printf("Read Subject %d completed, v_count: %d\n", I, Poly->Count);
#endif
        }

        u32 ClipsIdentifier = 0;
        fread(&ClipsIdentifier, sizeof(u32), 1, In);
        Assert(ClipsIdentifier == 0x0000FFFF);

        fread(&Clips->PolyCount, sizeof(u32), 1, In);
        Assert(Clips->PolyCount != 0);

        Clips->Polygons = (polygon *)malloc(sizeof(polygon)*Clips->PolyCount);
        for(u32 I = 0; I < Clips->PolyCount; ++ I)
        {
            polygon *Poly = Clips->Polygons + I;
            fread(&Poly->Count, sizeof(u32), 1, In);
            Assert(Poly->Count != 0);

            Poly->Points = (v2_f32 *)malloc(sizeof(v2_f32)*Poly->Count);
            
            fread(Poly->Points, sizeof(v2_f32)*Poly->Count, 1, In);
#if PRINT_READ
            printf("Read Clip %d completed, v_count: %d\n", I, Poly->Count);
#endif
        }
    }    

    fclose(In);
}

int main()
{
    BeginProfile();

    polygon_set Subjects = {};
    polygon_set Clips = {};

//    ReadPolies(&Subjects, &Clips, "d:/Clipper-2d/output/polygons_b.bin");
    ReadPolies(&Subjects, &Clips, "c:/Paul/Clipper-2d/output/polygons_b.bin");

    for(u32 I = 0; I < 262144; ++I)
    {
        polygon *S = Subjects.Polygons + I;
        polygon *C = Clips.Polygons + I;

//        polygon *S = Subjects.Polygons + 4;
//        polygon *C = Clips.Polygons + 4;

        IntersectTwoPolies(S, C, I);
    }

    EndAndPrintProfile();
    
    return 0;
}
