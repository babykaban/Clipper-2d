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

#define RECORD_BLOCKS 0
#include "test_clippers_profiler.cpp"

global_variable u32 IntersectCountMAX = 0;

#define PRINT_READ 0

#define TEST_INTERSECT 1
#define TEST_DIFFERENCE 1
#define TEST_UNION 1
#define TEST_XOR 1

#define PRINT_INTERSECT 0
#define PRINT_DIFFERENCE 0
#define PRINT_UNION 0
#define PRINT_XOR 0

#define TIME_ORIGINAL 0
#define TIME_NEW 0

inline void
PrintPolygons(u32 Count, polygon *Polygons)
{
    for(u32 polyi = 0;
        polyi < Count;
        ++polyi)
    {
        polygon *Poly = Polygons + polyi;
        printf("%d. ", polyi);
        for(u32 pi = 0;
            pi < (Poly->Count - 1);
            ++pi)
        {
            printf("(%.4f, %.4f), ", Poly->Points[pi].x, Poly->Points[pi].y);
        }

        printf("(%.4f, %.4f)\n", Poly->Points[Poly->Count - 1].x,
               Poly->Points[Poly->Count - 1].y);
    }
}

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

    srand((u32)time(0)); // Seed for randomness

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

//    ReadPolies(&Subjects, &Clips, "d:/Clipper-2d/output/polygons_b.bin");
    ReadPolies(&Subjects, &Clips, "c:/Paul/Clipper-2d/output/polygons_b.bin");
    printf("Avaliable to test %d\n", Subjects.PolyCount);

    FILE *file;
    fopen_s(&file, "records.csv", "w");
    
    u32 TestCount = 5;
    for(u32 TestIndex = 0;
        TestIndex < TestCount;
        ++TestIndex)
    {
        f32 MaxYf32 = 10000;
        f32 MaxXf32 = 10000;

        s32 PolygonCount = 4096;
        s32 numVertices = 16;
#if 1
        polygon_set SubjectSet = {};
        SubjectSet.PolyCount = PolygonCount;
        SubjectSet.Polygons = (polygon *)malloc(sizeof(polygon)*PolygonCount);
    
        u32 Count = 0;
        for(s32 I = 0;
            I < PolygonCount;
            ++I)
        {
            polygon *Poly = SubjectSet.Polygons + I;
            Poly->Count = rand() % (16 - 3 + 1)  + 3;
            Poly->Points = GenerateRandomPolygonF32(Poly->Count, -MaxXf32, MaxXf32, -MaxYf32, MaxYf32, Count++);
        }

        polygon_set ClipSet = {};
        ClipSet.PolyCount = PolygonCount;
        ClipSet.Polygons = (polygon *)malloc(sizeof(polygon)*PolygonCount);

        for(s32 I = 0;
            I < PolygonCount;
            ++I)
        {
            polygon *Poly = ClipSet.Polygons + I;
            Poly->Count = rand() % (16 - 3 + 1)  + 3;
            Poly->Points = GenerateRandomPolygonF32(Poly->Count, -MaxXf32, MaxXf32, -MaxYf32, MaxYf32, Count++);
        }
#endif
        for(s32 I = 0; I < PolygonCount; ++I)
        {
            CurrentOperationIndex = I;
            for(u32 FillRule = 0;
                FillRule < 1;
                ++FillRule)
            {
//                polygon *S = Subjects.Polygons + I;
//                polygon *C = Clips.Polygons + I;
                polygon *S = SubjectSet.Polygons + I;
                polygon *C = ClipSet.Polygons + I;
//            polygon *S = Subjects.Polygons + 131233;
//            polygon *C = Clips.Polygons + 131233;

                test_result Original = TESTTwoPoliesOriginal(S, C, FillRule, I);

                test_result New = TESTTwoPolies(S, C, FillRule, I);

                ApproveResults(&Original, &New);

#if RECORD_MEMORY_USEAGE
                Assert(MemoryAllocated == 0);
#endif
            }

            fprintf(stdout, "Pair [%d][%d] Tested\n", TestIndex,
                    CurrentOperationIndex);
        }
    }

#if 0
    for(u32 ClipType = 1;
        ClipType < ClipType_Count;
        ++ClipType)
    {
        for(u32 FillRule = 0;
            FillRule < FillRule_Count;
            ++FillRule)
        {
            for(u32 I = 0;
                I < ArrayCount(FunctionsToTest);
                ++I)
            {
                record_entry *EntryN =
                    Get(&OperationTables[0][ClipType][FillRule],
                        FunctionsToTest[I]);

                record_entry *EntryO =
                    Get(&OperationTables[1][ClipType][FillRule],
                        FunctionsToTest[I]);

                if(EntryN->Record.Count)
                {
                    f64 AveN = (f64)EntryN->Record.Average / (f64)EntryN->Record.Count;
                    f64 AveO = (f64)EntryO->Record.Average / (f64)EntryO->Record.Count;

                    printf("%s, %f, %d, %d\n", FunctionsToTest[I], AveN, ClipType, FillRule);
                    printf("%s, %f, %d, %d\n", FunctionsToTest[I], AveO, ClipType, FillRule);

                    printf("NCyc,OCyc,OPI,CT,FR,NVS,NVC\n");
                    fprintf(file, "NCyc,OCyc,OPI,CT,FR,NVS,NVC\n");
                    for(u32 BIndex = 0;
                        BIndex < EntryN->BlockIndex;
                        ++BIndex)
                    {
                        block_record *BRecord = EntryN->BlockRecords + BIndex;
                        block_record *BRecordO = EntryO->BlockRecords + BIndex;
                        printf("%.4f,%.4f,%u,%u,%u,%d,%d\n",
                               (f64)BRecord->Cycles / 10000.0,
                               (f64)BRecordO->Cycles / 10000.0,
                               BRecord->OpIndex,
                               BRecord->CT,
                               BRecord->FR,
                               Subjects.Polygons[BRecord->OpIndex].Count,
                               Clips.Polygons[BRecord->OpIndex].Count);

                        fprintf(file, "%.4f,%.4f,%u,%u,%u,%d,%d\n",
                                (f64)BRecord->Cycles / 10000.0,
                                (f64)BRecordO->Cycles / 10000.0,
                                BRecord->OpIndex,
                                BRecord->CT,
                                BRecord->FR,
                                Subjects.Polygons[BRecord->OpIndex].Count,
                                Clips.Polygons[BRecord->OpIndex].Count);
                    }
                }
            }
        }
    }
#endif

#if 0
    {
//        TimeBlock("Alloc Small");
        u8 *Mem0 = (u8 *)Malloc(sizeof(active));
    }

    {
//        TimeBlock("Alloc Big");
        u8 *Mem1 = (u8 *)Malloc(sizeof(active)*4096);
    }
#endif
    
    fclose(file);
    EndAndPrintProfile();


    
    return(0);
}
