/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */

struct test_clock
{
    char const *Label;
    f64 Cicles;

    u32 Index;
};

global_variable test_clock Max[4096];
global_variable test_clock Min[4096];

internal void
RecordMinMax(u32 OperationIndex)
{
    for(u32 AnchorIndex = 0; AnchorIndex < ArrayCount(GlobalProfilerAnchors); ++AnchorIndex)
    {
        profile_anchor *Anchor = GlobalProfilerAnchors + AnchorIndex;
        if(Anchor->TSCElapsedInclusive)
        {
            f64 Average = (f64)Anchor->TSCElapsedExclusive / (f64)Anchor->HitCount;

            if(Max[AnchorIndex].Label)
            {
                if(Max[AnchorIndex].Cicles < Average)
                {
                    Max[AnchorIndex].Cicles = Average;            
                    Max[AnchorIndex].Index = OperationIndex;            
                }
            }
            else
            {
                Max[AnchorIndex].Label = Anchor->Label;
                Max[AnchorIndex].Cicles = Average;            
                Max[AnchorIndex].Index = OperationIndex;            
            }

            if(Min[AnchorIndex].Label)
            {
                if(Min[AnchorIndex].Cicles > Average)
                {
                    Min[AnchorIndex].Cicles = Average;            
                    Min[AnchorIndex].Index = OperationIndex;            
                }
            }
            else
            {
                Min[AnchorIndex].Label = Anchor->Label;
                Min[AnchorIndex].Cicles = Average;            
                Min[AnchorIndex].Index = OperationIndex;            
            }
        }
    }
}

internal void
PrintMinMax(void)
{
    for(u32 I = 0; I < ArrayCount(GlobalProfilerAnchors); ++I)
    {
        if(Min[I].Label)
        {
            printf("MIN[%d]: %s, %.2f\n", Min[I].Index, Min[I].Label, Min[I].Cicles);
        }

        if(Max[I].Label)
        {
            printf("MAX[%d]: %s, %.2f\n", Max[I].Index, Max[I].Label, Max[I].Cicles);
        }
    }
}
