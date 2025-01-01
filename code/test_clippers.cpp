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

int main()
{
    BeginProfile();

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
        u32 Count = 262144;
        for(u32 I = 0; I < Count; ++I)
        {
            polygon *S = Subjects.Polygons + I;
            polygon *C = Clips.Polygons + I;
        
            TESTTwoPoliesOriginal(S, C, I);
            TESTTwoPolies(S, C, I);

            Assert(MemoryAllocated == 0);

            u64 TotalCPUElapsed = GlobalProfiler.EndTSC - GlobalProfiler.StartTSC;
            PrintAnchorData(TotalCPUElapsed);

            RecordMinMax(I);
        }
    }

    PrintMinMax();
    EndAndPrintProfile();

    return(0);
}
