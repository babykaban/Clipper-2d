/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */

#define PROFILER 1
#define PRINT_OUT_RESULT 1

#include "main.h"
#include "clipper.h"

#include "profiler.cpp"
#include "repetition_tester.cpp"
#include "clipper.cpp"

#define PRINT_READ 0

internal s32
BooleanOpD(uint8_t cliptype, uint8_t fillrule, paths_f64 *subjects, paths_f64 *subjects_open,
           paths_f64 *clips, paths_f64 *solution, paths_f64 *solution_open,
           b32 preserve_collinear, b32 reverse_solution)
{
    TimeFunction;
    
    if (cliptype > ClipType_Xor)
    {
        Assert(!"Out of range");
        return -4;
    }

    if (fillrule > FillRule_Negative)
    {
        Assert(!"Out of range");
        return -3;
    }

    clipper Clipper = {};
    InitClipper(&Clipper, 2);
    
    Clipper.PreserveCollinear = preserve_collinear;
    Clipper.ReverseSolution = reverse_solution;

    if(subjects->PathCount > 0)
        AddSubjects(&Clipper, subjects);

    if(subjects_open)
    {
        if(subjects_open->PathCount > 0)
            AddOpenSubjects(&Clipper, subjects_open);
    }

    if(clips->PathCount > 0)
        AddClips(&Clipper, clips);

    if(!Execute(&Clipper, (clip_type)cliptype, (fill_rule)fillrule, solution, solution_open))
    {
        Assert(!"clipping bug - should never happen :)");
        return -1; // clipping bug - should never happen :)
    }

    return 0; //success !!
}

inline void
IntersectTwoPolies(polygon *S, polygon *C)
{
    TimeFunction;

    Assert((S->Count > 0) && (C->Count > 0));

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
    BooleanOpD(ClipType_Intersection, FillRule_EvenOdd, &Subject, 0, &Clip, &Solution, 0, true, false);
    
#if PRINT_OUT_RESULT
    printf("\nTest: %s, %s", ClipTypes[ClipType_Intersection], FillRules[FillRule_EvenOdd]);

    for(s32 PC = 0;
        PC < Solution.PathCount;
        ++PC)
    {
        printf("\n%d. ", PC);
        path_f64 *Path = Solution.Paths + PC;
        for(s32 J = 0;
            J < (Path->Count - 1);
            ++J)
        {
            printf("(%.2f, %.2f), ", Path->Points[J].x, Path->Points[J].y);
        }

        printf("(%.2f, %.2f)", Path->Points[Path->Count - 1].x, Path->Points[Path->Count - 1].y);
    }

    printf("\n");
#endif

    FreePaths(&Solution);
    FreePaths(&Subject);
    FreePaths(&Clip);
}

inline void
UnionTwoPolies(polygon *S, polygon *C)
{
}

inline void
DifferenciateTwoPolies(polygon *S, polygon *C)
{
}


inline void
XorTwoPolies(polygon *S, polygon *C)
{
}

internal void
TestBooleanOp(repetition_tester *Tester, paths_f64 *subjects, paths_f64 *subjects_open, paths_f64 *clips,
              paths_f64 *solution, paths_f64 *solution_open)
{
    while(IsTesting(Tester))
    {
        u32 Count = 0;
        for(u8 ClipType = 0;
            ClipType < ClipType_Count;
            ++ClipType)
        {
            for(u8 FillRule = 0;
                FillRule < FillRule_Count;
                ++FillRule)
            {
#if PRINT_OUT_RESULT
                printf("\nTest: %s, %s", ClipTypes[ClipType], FillRules[FillRule]);
#endif
                paths_f64 Dummy = {};
                BeginTime(Tester);
                BooleanOpD(ClipType, FillRule, subjects, &Dummy, clips, solution, &Dummy, true, false);
                EndTime(Tester);
                
#if PRINT_OUT_RESULT
                for(s32 PC = 0;
                    PC < solution->PathCount;
                    ++PC)
                {
                    printf("\n%d. ", PC);
                    path_f64 *Path = solution->Paths + PC;
                    for(s32 J = 0;
                        J < (Path->Count - 1);
                        ++J)
                    {
                        printf("(%.2f, %.2f), ", Path->Points[J].x, Path->Points[J].y);
                    }

                    printf("(%.2f, %.2f)", Path->Points[Path->Count - 1].x, Path->Points[Path->Count - 1].y);
                }

                printf("\n");
#endif
            }
        }
    }
}

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

//#include "clipper2/clipper.h"
//#include "clipper2/clipper.engine.cpp"

int main()
{
    BeginProfile();

    if(check_avx2_support())
    {
        printf("Support wide registers from AVX2 and lower");
    }
    else if(check_sse3_support())
    {
        printf("Support wide registers from SSE3 and lower");
    }
    else if(check_sse2_support())
    {
        printf("Support wide registers from SSE2 and lower");
    }
    else if(check_sse_support())
    {
        printf("Support wide registers SSE only");
    }
    else
    {
        printf("No wide registers support");
    }

    polygon_set Subjects = {};
    polygon_set Clips = {};

//    ReadPolies(&Subjects, &Clips, "d:/Clipper-2d/output/polygons_b.bin");
    ReadPolies(&Subjects, &Clips, "c:/Paul/Clipper-2d/output/polygons_b.bin");

    for(u32 I = 0; I < 1; ++I)
    {
//        polygon *S = Subjects.Polygons + I;
//        polygon *C = Clips.Polygons + I;

        polygon *S = Subjects.Polygons + 4;
        polygon *C = Clips.Polygons + 4;

        IntersectTwoPolies(S, C);
    }

#if 0
    u64 CPUTimerFreq = EstimateCPUTimerFreq();

    repetition_tester Tester = {};
    NewTestWave(&Tester, 0, CPUTimerFreq);

    TestBooleanOp(&Tester, &Subject, 0, &Clip, &Solution, 0);
#endif
    
#if 0
    printf("Intersection: ");
    for(s32 I = 0;
        I < Result.PathCount;
        ++I)
    {
        path_f64 *Path = Result.Paths + I;
        for(s32 J = 0;
            J < (Path->Count - 1);
            ++J)
        {
            if((J % 4) == 0)
            {
                printf("\n");
            }

            printf("{%.2f, %.2f}, ", Path->Points[J].x, Path->Points[J].y);
        }

        printf("{%.2f, %.2f};", Path->Points[Path->Count - 1].x, Path->Points[Path->Count - 1].y);
    }

    printf("\n\n");
#endif    
    EndAndPrintProfile();
        
    return(0);
}
