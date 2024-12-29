/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */
#include "main.h"
#include "clipper_memory.h"

#define PROFILER 1
#include "profiler.cpp"

#if 0
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
    TimeFunction;
    
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
    TimeFunction;

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

int main()
{
    BeginProfile();

    sort_entry points[COUNT] = {};
    intersect_node nodes[COUNT] = {};

    for(s32 i = 0; i < COUNT; ++i)
    {
        points[i].pt = generateRandomPoint(MIN, MAX);
    }

    for(s32 i = 0; i < COUNT; ++i)
    {
        nodes[i].pt = points[i].pt;
    }

    
    // Calculate SortKeys (with shifting)
    s64 minX = INT_MAX, minY = INT_MAX;
    for (s32 i = 0; i < COUNT; ++i)
    {
        minX = Minimum(minX, points[i].pt.x);
        minY = Minimum(minY, points[i].pt.y);
    }
    
    for (s32 i = 0; i < COUNT; ++i) 
    {
        points[i].SortKey = SortKeyToU64(points[i].pt);
    }

    sort_entry temp[COUNT]; 
    RadixSort(COUNT, points, temp); 

    MergeSort(COUNT, nodes);

    // Print the sorted points
    for (s32 i = 0; i < COUNT; ++i) {
        printf("(%lld, %lld) ", points[i].pt.x, points[i].pt.y);
    }
    printf("\n\n");

    for (s32 i = 0; i < COUNT; ++i) {
        printf("(%lld, %lld) ", nodes[i].pt.x, nodes[i].pt.y);
    }
    printf("\n");

    EndAndPrintProfile();

    return 0;
}
#endif

#include <stdio.h>
#include <stdint.h>

uint64_t generate_key(int64_t x, int64_t y) {
    const int64_t BIAS = 1LL << 30; // Bias to handle negatives
    const uint64_t MASK_31_BITS = 0x7FFFFFFF;
    uint64_t y_part = ((uint64_t)(y + BIAS) & MASK_31_BITS) << 31;
    uint64_t x_part = ((uint64_t)(x + BIAS) & MASK_31_BITS);
    return y_part | x_part;
}

void decode_key(uint64_t key, int64_t *x, int64_t *y) {
    const int64_t BIAS = 1LL << 30;
    const uint64_t MASK_31_BITS = 0x7FFFFFFF;
    *x = (int64_t)(key & MASK_31_BITS) - BIAS;
    *y = (int64_t)((key >> 31) & MASK_31_BITS) - BIAS;
}

int main() {
    int64_t x = 3, y = -2;

    // Generate key
    uint64_t key = generate_key(x, y);
    printf("Encoded key: %llu\n", key);

    // Decode key
    int64_t decoded_x, decoded_y;
    decode_key(key, &decoded_x, &decoded_y);
    printf("Decoded values: x = %lld, y = %lld\n", decoded_x, decoded_y);

    return 0;
}