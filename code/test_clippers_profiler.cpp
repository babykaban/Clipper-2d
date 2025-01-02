/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */
#include "test_clippers_profiler.h"

internal void
RecordMinMax(clock_record *Record, u64 Elapsed, u32 OpIndex)
{
    if(Record->Min > Elapsed)
    {
        Record->Min = Elapsed;
        Record->MinIndex = OpIndex;
    }

    if(Record->Max < Elapsed)
    {
        Record->Max = Elapsed;
        Record->MaxIndex = OpIndex;
    }

    ++Record->Count;
    Record->Average += Elapsed;
}

internal void
PrintMinMax(clock_record *Record)
{
    u64 CPUFreq = EstimateCPUTimerFreq();

    f64 Ave = (f64)Record->Average / (f64)Record->Count;
    
    f64 MinMs = 1000.0 * ((f64)Record->Min / (f64)CPUFreq);
    f64 MaxMs = 1000.0 * ((f64)Record->Max / (f64)CPUFreq);
    f64 AveMs = 1000.0 * ((f64)Ave / (f64)CPUFreq);

    printf("MIN[%d]: %s, %llu cyc, %.2f ms\n", Record->MinIndex, Record->Label,
           Record->Min, MinMs);
    printf("MAX[%d]: %s, %llu cyc, %.2f ms\n", Record->MaxIndex, Record->Label,
           Record->Max, MaxMs);
    printf("AVERAGE: %s, %.2f cyc, %.2f ms\n", Record->Label, Ave, AveMs);
}
