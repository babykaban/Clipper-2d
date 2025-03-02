/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */

#include "clipper_main.h"
#include "clipper.h"
#include "clipper.cpp"

internal void
ReadPolies(polygon_set *Subjects, polygon_set *Clips, char *FileName)
{
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

int
main()
{
    polygon_set Subjects = {};
    polygon_set Clips = {};

    ReadPolies(&Subjects, &Clips, "d:/Clipper-2d/output/polygons_b.bin");

    polygon *S = Subjects.Polygons + 0;
    polygon *C = Clips.Polygons + 0;
    printf("Subject:");
    for(u32 PointIndex = 0;
        PointIndex < S->Count;
        ++PointIndex)
    {
        v2_f32 Point = S->Points[PointIndex];
        printf(" (%.4f, %.4f),", Point.x, Point.y);
    }

    printf("\nClip:");
    for(u32 PointIndex = 0;
        PointIndex < C->Count;
        ++PointIndex)
    {
        v2_f32 Point = C->Points[PointIndex];
        printf(" (%.4f, %.4f),", Point.x, Point.y);
    }
    printf("\n\n");
    
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
    BooleanOpD(ClipType_Difference, FillRule_EvenOdd, &Subject, 0, &Clip, &Solution, 0, true, false);

    printf("Difference\n");
    for(s32 PathIndex = 0;
        PathIndex < Solution.PathCount;
        ++PathIndex)
    {
        path_f64 *P = Solution.Paths + PathIndex;
        printf("Path[%d]:", PathIndex);
        for(s32 PointIndex = 0;
            PointIndex < P->Count;
            ++PointIndex)
        {
            v2_f64 Point = P->Points[PointIndex];
            printf(" (%.4f, %.4f),", Point.x, Point.y);
        }
        printf("\n\n");
    }
    FreePaths(&Solution);


    BooleanOpD(ClipType_Intersection, FillRule_EvenOdd, &Subject, 0, &Clip, &Solution, 0, true, false);

    printf("Intersection\n");
    for(s32 PathIndex = 0;
        PathIndex < Solution.PathCount;
        ++PathIndex)
    {
        path_f64 *P = Solution.Paths + PathIndex;
        printf("Path[%d]:", PathIndex);
        for(s32 PointIndex = 0;
            PointIndex < P->Count;
            ++PointIndex)
        {
            v2_f64 Point = P->Points[PointIndex];
            printf(" (%.4f, %.4f),", Point.x, Point.y);
        }
        printf("\n\n");
    }
    FreePaths(&Solution);

    BooleanOpD(ClipType_Union, FillRule_EvenOdd, &Subject, 0, &Clip, &Solution, 0, true, false);

    printf("Union\n");
    for(s32 PathIndex = 0;
        PathIndex < Solution.PathCount;
        ++PathIndex)
    {
        path_f64 *P = Solution.Paths + PathIndex;
        printf("Path[%d]:", PathIndex);
        for(s32 PointIndex = 0;
            PointIndex < P->Count;
            ++PointIndex)
        {
            v2_f64 Point = P->Points[PointIndex];
            printf(" (%.4f, %.4f),", Point.x, Point.y);
        }
        printf("\n\n");
    }
    FreePaths(&Solution);


    BooleanOpD(ClipType_Xor, FillRule_EvenOdd, &Subject, 0, &Clip, &Solution, 0, true, false);

    printf("Xor\n");
    for(s32 PathIndex = 0;
        PathIndex < Solution.PathCount;
        ++PathIndex)
    {
        path_f64 *P = Solution.Paths + PathIndex;
        printf("Path[%d]:", PathIndex);
        for(s32 PointIndex = 0;
            PointIndex < P->Count;
            ++PointIndex)
        {
            v2_f64 Point = P->Points[PointIndex];
            printf(" (%.4f, %.4f),", Point.x, Point.y);
        }
        printf("\n\n");
    }
    FreePaths(&Solution);


}
