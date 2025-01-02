#if !defined(TEST_CLIPPERS_PROFILER_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

struct clock_record
{
    char const *Label;

    u64 Min;
    u64 Max;
    
    u32 MinIndex;
    u32 MaxIndex;

    u32 Count;
    u64 Average;
};

global_variable u32 RecordCount = 0;
global_variable clock_record Records[4][4][4096];


struct parent_clock_record
{
    u32 ClipType;
    u32 FillRule;

    clock_record Children[4096];
};

#define TEST_CLIPPERS_PROFILER_H
#endif
