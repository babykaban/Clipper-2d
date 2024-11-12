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

global_variable char *ClipTypes[] =
{
    "ClipType_NoClip",
    "ClipType_Intersection",
    "ClipType_Union",
    "ClipType_Difference",
    "ClipType_Xor",
};

global_variable char *FillRules[] =
{
    "FillRule_EvenOdd",
    "FillRule_NonZero",
    "FillRule_Positive",
    "FillRule_Negative",
};

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

int main()
{
    BeginProfile();
    
    paths_f64 Subject = GetPathsF64(2);
    Subject.PathCount = 2;

    paths_f64 Clip = GetPathsF64(1);
    Clip.PathCount = 1;

    Subject.Paths[0] = GetPathF64(4);
    Subject.Paths[0].Count = 4;
    Subject.Paths[0].Points[0] = {0, 0};
    Subject.Paths[0].Points[1] = {100, 0};
    Subject.Paths[0].Points[2] = {100, 100};
    Subject.Paths[0].Points[3] = {0, 100};

    Subject.Paths[1] = GetPathF64(4);
    Subject.Paths[1].Count = 4;
    Subject.Paths[1].Points[0] = {30, 30};
    Subject.Paths[1].Points[1] = {70, 30};
    Subject.Paths[1].Points[2] = {70, 70};
    Subject.Paths[1].Points[3] = {30, 70};

    Clip.Paths[0] = GetPathF64(4);
    Clip.Paths[0].Count = 4;
    Clip.Paths[0].Points[0] = {50, -20};
    Clip.Paths[0].Points[1] = {120, -20};
    Clip.Paths[0].Points[2] = {120, 80};
    Clip.Paths[0].Points[3] = {50, 80};

//    paths_f64 Result = BooleanOp(ClipType_Intersection, FillRule_EvenOdd, &Subject, &Clip);

    
    paths_f64 Solution = {};
    paths_f64 Dummy = {};
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
