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

inline b32
CheckPoints(polygon *N, polygon *O)
{
    b32 Result = true; 

    u32 Count = 0;
    while(!PointsAreEqual(O->Points[0], N->Points[0]))
    {
        v2_f32 First = N->Points[0];
        for(u32 I = 0;
            I < (N->Count - 1);
            ++I)
        {
            N->Points[I] = N->Points[I + 1];
        }
        N->Points[N->Count - 1] = First;

        ++Count;
        if(Count > N->Count)
        {
            break;
        }
    }

    for(u32 I = 0;
        I < N->Count;
        ++I)
    {
        v2_f32 NP = N->Points[I];
        v2_f32 OP = O->Points[I];
        v2_f32 Diff = {OP.x - NP.x, OP.y - NP.y};

        if(!((Diff.x == 0.0) && (Diff.y  == 0.0)))
        {
            Result = false;
            break;
        }
    }

    return(Result);
}

inline b32
CheckPoly(polygon *N, polygon_set *Os)
{
    b32 Result = false;

    for(u32 OPolyIndex = 0;
        OPolyIndex < Os->PolyCount;
        ++OPolyIndex)
    {
        polygon *OldPoly = Os->Polygons + OPolyIndex;
        if(OldPoly->Count == N->Count)
        {
            Result = CheckPoints(N, OldPoly);
            if(Result)
            {
                break;
            }
        }
        else
        {
            Result = false;
        }
    }

    return(Result);
}

internal void
ApproveResults(polygon_set *New, polygon_set *Old)
{
    Assert(New->PolyCount == Old->PolyCount);

    b32 PolyFound = true;
    for(u32 NPolyIndex = 0;
        NPolyIndex < New->PolyCount;
        ++NPolyIndex)
    {
        polygon *NewPoly = New->Polygons + NPolyIndex;
        if(!CheckPoly(NewPoly, Old))
        {
            Assert("FATAL");
        }
    }

    for(u32 PolyIndex = 0;
        PolyIndex < New->PolyCount;
        ++PolyIndex)
    {
        polygon *OriginalPoly = Old->Polygons + PolyIndex;
        polygon *NewPoly = New->Polygons + PolyIndex;

        free(OriginalPoly->Points);
        free(NewPoly->Points);
    }

    free(Old->Polygons);
    free(New->Polygons);
}

internal void
ApproveResults(test_result *Original, test_result *New, f32 Epsilon = 0.01f)
{
    TimeFunction;
    
    Assert(Original->Dif.PolyCount == New->Dif.PolyCount);
    ApproveResults(&New->Dif, &Original->Dif);
    
    Assert(Original->Inter.PolyCount == New->Inter.PolyCount);
    ApproveResults(&New->Inter, &Original->Inter);
    
    Assert(Original->Union.PolyCount == New->Union.PolyCount);
    ApproveResults(&New->Union, &Original->Union);

    Assert(Original->Xor.PolyCount == New->Xor.PolyCount);
    ApproveResults(&New->Xor, &Original->Xor);
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
    printf("Avaliable to test %d\n", Subjects.PolyCount);
    
    u32  TestCount = 1;
    for(u32 TestIndex = 0;
        TestIndex < TestCount;
        ++TestIndex)
    {
        u32 Count = 262144;
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

#if 0
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
#endif
    EndAndPrintProfile();
    return(0);
}
