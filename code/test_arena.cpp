/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */

#define PROFILER 1
#include "main.h"

#include "profiler.cpp"

#include "clipper_memory.h"
#include "clipper_math.h"
#include "clipper_math_f64.h"
#include "clipper_math_s64.h"

#if 1
struct point 
{
    s64 x;
    s64 y;
};

struct sort_entry
{
    point pt; 
    u64 SortKey; 
};

u64
SortKeyToU64(point pt)
{
    if (pt.y >= 0) {
        return (((uint64_t)(pt.y) << 32) | pt.x);
    } else {
        return (((uint64_t)(INT_MAX - pt.y) << 32) | pt.x); 
    }
}

void RadixSort(u32 Count, sort_entry *First, sort_entry *Temp)
{
    sort_entry *Source = First;
    sort_entry *Dest = Temp;
    for(u32 ByteIndex = 0;
        ByteIndex < 64; // Iterate through all 8 bytes of the u64 SortKey
        ByteIndex += 8)
    {
        u32 SortKeyOffsets[256] = {};

        // First pass - count how many of each key
        for(u32 Index = 0; Index < Count; ++Index)
        {
            u64 RadixValue = Source[Index].SortKey;
            u32 RadixPiece = (RadixValue >> ByteIndex) & 0xFF;
            ++SortKeyOffsets[RadixPiece];
        }

        // Change counts to offsets
        u32 Total = 0;
        for(u32 SortKeyIndex = 0; SortKeyIndex < 256; ++SortKeyIndex)
        {
            u32 KeyCount = SortKeyOffsets[SortKeyIndex];
            SortKeyOffsets[SortKeyIndex] = Total;
            Total += KeyCount;
        }

        // Second pass - place elements into the right location
        for(u32 Index = 0; Index < Count; ++Index)
        {
            u64 RadixValue = Source[Index].SortKey;
            u32 RadixPiece = (RadixValue >> ByteIndex) & 0xFF;
            Dest[SortKeyOffsets[RadixPiece]++] = Source[Index];
        }

        sort_entry *SwapTemp = Dest;
        Dest = Source;
        Source = SwapTemp;
    }
}

struct intersect_node
{
    point pt;
};

inline b32
IntersectListSort(intersect_node *a, intersect_node *b)
{
    //note different inequality tests ...
    return (a->pt.y == b->pt.y) ? (a->pt.x < b->pt.x) : (a->pt.y > b->pt.y);
}

inline void
Swap(intersect_node *A, intersect_node *B)
{
    intersect_node Temp = *B;
    *B = *A;
    *A = Temp;
}

internal void
MergeSort(u32 Count, intersect_node *First)
{
    intersect_node *Temp = (intersect_node *)malloc(sizeof(intersect_node)*Count);

    if(Count == 1)
    {
        // NOTE(casey): No work to do.
    }
    else if(Count == 2)
    {
        intersect_node *EntryA = First;
        intersect_node *EntryB = First + 1;
        if(IntersectListSort(EntryA, EntryB))
        {
            Swap(EntryA, EntryB);
        }
    }
    else
    {
        u32 Half0 = Count / 2;
        u32 Half1 = Count - Half0;

        Assert(Half0 >= 1);
        Assert(Half1 >= 1);

        intersect_node *InHalf0 = First;
        intersect_node *InHalf1 = First + Half0;
        intersect_node *End = First + Count;

        MergeSort(Half0, InHalf0);
        MergeSort(Half1, InHalf1);

        intersect_node *ReadHalf0 = InHalf0;
        intersect_node *ReadHalf1 = InHalf1;

        intersect_node *Out = Temp;
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            if(ReadHalf0 == InHalf1)
            {
                *Out++ = *ReadHalf1++;
            }
            else if(ReadHalf1 == End)
            {
                *Out++ = *ReadHalf0++;
            }
            else if(!IntersectListSort(ReadHalf0, ReadHalf1))
            {
                *Out++ = *ReadHalf0++;
            }
            else
            {
                *Out++ = *ReadHalf1++;
            }            
        }
        Assert(Out == (Temp + Count));
        Assert(ReadHalf0 == InHalf1);
        Assert(ReadHalf1 == End);
            
        Copy(sizeof(intersect_node)*Count, Temp, First);
    }

    free(Temp);
}

point
generateRandomPoint(s64 minVal, s64 maxVal) {

    point p;
    p.x = rand() % (maxVal - minVal + 1) + minVal;
    p.y = rand() % (maxVal - minVal + 1) + minVal;

    return p;
}

#define COUNT 16
#define MIN -1000
#define MAX 1000

inline void
InsertionSort(s32 Count, intersect_node *Nodes)
{
    for(s32 I = 1; I < Count; ++I)
    {
        intersect_node *Node = Nodes + I;
        s32 J = I - 1;

        while((J >= 0) && IntersectListSort(Nodes + J, Node))
        {
            Nodes[J + 1] = Nodes[J];
            J = J - 1;
        }

        Nodes[J + 1] = *Node;
    }
}

int main()
{
    BeginProfile();
    
    EndAndPrintProfile();

    return 0;
}
#endif
