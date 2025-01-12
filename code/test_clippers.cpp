/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */

#include "main.h"

#define PRINT_OUT_RESULT 0
#define PROFILER 1
#include "profiler.cpp"

#include "test_clippers_profiler.cpp"

global_variable u32 IntersectCountMAX = 0;
#define PRINT_READ 0

#include "original_clipper.cpp"
#include "renewed_clipper.cpp"

#include "generate.cpp"

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

internal void
ApproveResults(test_result *Original, test_result *New, f32 Epsilon = 0.01f)
{
    Assert(Original->Dif.PolyCount == New->Dif.PolyCount);
    for(u32 PolyIndex = 0;
        PolyIndex < Original->Dif.PolyCount;
        ++PolyIndex)
    {
        polygon *OriginalPoly = Original->Dif.Polygons + PolyIndex;
        polygon *NewPoly = New->Dif.Polygons + PolyIndex;

        Assert(OriginalPoly->Count == NewPoly->Count);
        for(u32 PointIndex = 0;
            PointIndex < OriginalPoly->Count;
            ++PointIndex)
        {
            v2_f32 OP = OriginalPoly->Points[PointIndex];
            v2_f32 NP = NewPoly->Points[PointIndex];
            v2_f32 Diff = {OP.x - NP.x, OP.y - NP.y};

            Assert((Diff.x < Epsilon) && (Diff.y < Epsilon));
        }

        free(OriginalPoly->Points);
        free(NewPoly->Points);
    }
    free(Original->Dif.Polygons);
    free(New->Dif.Polygons);
    
    Assert(Original->Inter.PolyCount == New->Inter.PolyCount);
    for(u32 PolyIndex = 0;
        PolyIndex < Original->Inter.PolyCount;
        ++PolyIndex)
    {
        polygon *OriginalPoly = Original->Inter.Polygons + PolyIndex;
        polygon *NewPoly = New->Inter.Polygons + PolyIndex;

        Assert(OriginalPoly->Count == NewPoly->Count);
        for(u32 PointIndex = 0;
            PointIndex < OriginalPoly->Count;
            ++PointIndex)
        {
            v2_f32 OP = OriginalPoly->Points[PointIndex];
            v2_f32 NP = NewPoly->Points[PointIndex];
            v2_f32 Diff = {OP.x - NP.x, OP.y - NP.y};

            Assert((Diff.x < Epsilon) && (Diff.y < Epsilon));
        }

        free(OriginalPoly->Points);
        free(NewPoly->Points);
    }
    free(Original->Inter.Polygons);
    free(New->Inter.Polygons);

    Assert(Original->Union.PolyCount == New->Union.PolyCount);
    for(u32 PolyIndex = 0;
        PolyIndex < Original->Union.PolyCount;
        ++PolyIndex)
    {
        polygon *OriginalPoly = Original->Union.Polygons + PolyIndex;
        polygon *NewPoly = New->Union.Polygons + PolyIndex;

        Assert(OriginalPoly->Count == NewPoly->Count);
        for(u32 PointIndex = 0;
            PointIndex < OriginalPoly->Count;
            ++PointIndex)
        {
            v2_f32 OP = OriginalPoly->Points[PointIndex];
            v2_f32 NP = NewPoly->Points[PointIndex];
            v2_f32 Diff = {OP.x - NP.x, OP.y - NP.y};

            Assert((Diff.x < Epsilon) && (Diff.y < Epsilon));
        }

        free(OriginalPoly->Points);
        free(NewPoly->Points);
    }
    free(Original->Union.Polygons);
    free(New->Union.Polygons);

    Assert(Original->Xor.PolyCount == New->Xor.PolyCount);
    for(u32 PolyIndex = 0;
        PolyIndex < Original->Xor.PolyCount;
        ++PolyIndex)
    {
        polygon *OriginalPoly = Original->Xor.Polygons + PolyIndex;
        polygon *NewPoly = New->Xor.Polygons + PolyIndex;

        Assert(OriginalPoly->Count == NewPoly->Count);
        for(u32 PointIndex = 0;
            PointIndex < OriginalPoly->Count;
            ++PointIndex)
        {
            v2_f32 OP = OriginalPoly->Points[PointIndex];
            v2_f32 NP = NewPoly->Points[PointIndex];
            v2_f32 Diff = {OP.x - NP.x, OP.y - NP.y};

            Assert((Diff.x < Epsilon) && (Diff.y < Epsilon));
        }

        free(OriginalPoly->Points);
        free(NewPoly->Points);
    }
    free(Original->Xor.Polygons);
    free(New->Xor.Polygons);
}

int main()
{
    BeginProfile();

    SetUpHashTables();
    
    if(check_avx2_support())
    {
        AVX2_SUPPORTED = true;
        printf("Support wide registers from AVX2 and lower\n");
    }
    else if(check_avx_support())
    {
        AVX_SUPPORTED = true;
        printf("Support wide registers from AVX and lower\n");
    }
    else if(check_sse3_support())
    {
        SSE3_SUPPORTED = true;
        printf("Support wide registers from SSE3 and lower\n");
    }
    else if(check_sse2_support())
    {
        SSE2_SUPPORTED = true;
        printf("Support wide registers from SSE2 and lower\n");
    }
    else if(check_sse_support())
    {
        SSE_SUPPORTED = true;
        printf("Support wide registers SSE only\n");
    }
    else
    {
        printf("No wide registers support\n");
    }

    polygon_set Subjects = {};
    polygon_set Clips = {};

    ReadPolies(&Subjects, &Clips, "d:/Clipper-2d/output/polygons_b.bin");
//    ReadPolies(&Subjects, &Clips, "c:/Paul/Clipper-2d/output/polygons_b.bin");

    
    u32  TestCount = 1;
    for(u32 TestIndex = 0;
        TestIndex < TestCount;
        ++TestIndex)
    {
        u32 Count = 262144 / 8;
//        u32 Count = 1024;
        for(u32 I = 0; I < Count; ++I)
        {
            CurrentOperationIndex = I;

            polygon *S = Subjects.Polygons + I;
            polygon *C = Clips.Polygons + I;

            test_result Original = TESTTwoPoliesOriginal(S, C, I);

            test_result New = TESTTwoPolies(S, C, I);

            ApproveResults(&Original, &New);

            Assert(MemoryAllocated == 0);
        }
    }

    char *SlowestKey = 0;
    f64 SlowestAverage = 0;
    u32 ct = 0;
    u32 fr = 0;
    for(u32 ClipType = 1;
        ClipType < ClipType_Count;
        ++ClipType)
    {
        printf("<===%s===\n", ClipTypes[ClipType]);
        printf("    ");

        for(u32 FillRule = 0;
            FillRule < FillRule_Count;
            ++FillRule)
        {
            printf(">===%s===\n", FillRules[FillRule]);
            printf("    ");
            for(u32 I = 0;
                I < ArrayCount(FunctionsToTest);
                ++I)
            {
                clock_record *Record =
                    Get(&OperationTables[ClipType_Difference][FillRule_EvenOdd],
                        FunctionsToTest[I]);

                PrintRecord(FunctionsToTest[I], Record);
                printf("\n");

                f64 Ave = (f64)Record->Average / (f64)Record->Count;
                if(Ave > SlowestAverage)
                {
                    SlowestAverage = Ave;
                    SlowestKey = FunctionsToTest[I]; 
                    ct = ClipType;
                    fr = FillRule;
                }
            }

            printf("==============<\n\n");
        }

        printf("==============>\n\n");
    }

    printf("<===%s===\n", ClipTypes[ct]);
    printf(">===%s===\n", FillRules[fr]);
    clock_record *Record = Get(&OperationTables[ct][fr], SlowestKey);
    PrintRecord(SlowestKey, Record);

    EndAndPrintProfile();
    return(0);
}
