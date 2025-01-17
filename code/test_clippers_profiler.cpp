/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */
#include "test_clippers_profiler.h"

inline clock_record
InitClockRecord(void)
{
    clock_record Result {};
    Result.Min = INT_MAX;

    return(Result);
}

global_variable char *FunctionsToTest[8] =
{
    "ExecuteInternal",
    "BuildPathsD",

    "InsertLocalMinimaIntoAEL",
    "DoHorizontal",
    "BuildIntersectList",
    "ProcessIntersectList",
    "DoTopOfScanbeam",
    "ProcessHorzJoins"
};

internal void
SetUpHashTables(void)
{

    InitHashTables();
    
    for(u32 K = 0; K < 2; ++K)
    {
        for(u32 I = 0; I < 5; ++I)
        {
            for(u32 J = 0; J < 4; ++J)
            {
                record_hash_table *Table = &OperationTables[K][I][J];
                clock_record Record = InitClockRecord();
                for(u32 F = 0; F < ArrayCount(FunctionsToTest); ++F)
                {
                    Insert(Table, FunctionsToTest[F], Record);
                }
            }
        }
    }
}
