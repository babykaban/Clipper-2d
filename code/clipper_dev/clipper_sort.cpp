/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */

inline void
Swap(local_minima *A, local_minima *B)
{
    local_minima Temp = *B;
    *B = *A;
    *A = Temp;
}

inline void
Swap(horz_segment *A, horz_segment *B)
{
    horz_segment Temp = *B;
    *B = *A;
    *A = Temp;
}

inline void
Swap(intersect_node *A, intersect_node *B)
{
    intersect_node Temp = *B;
    *B = *A;
    *A = Temp;
}

inline b32
LocMinSorter(local_minima *A, local_minima *B)
{
    b32 Result = false;

    if(B->Vertex->P.y != A->Vertex->P.y)
    {
        Result = (B->Vertex->P.y < A->Vertex->P.y);
    }
    else
    {
        Result = (B->Vertex->P.x > A->Vertex->P.x);
    }

    return(!Result);
}

inline b32
HorzSegSorter(horz_segment *hs1, horz_segment *hs2)
{
    if(!hs1->right_op || !hs2->right_op)
        return !(hs1->right_op ? 1 : 0);

    return !(hs2->left_op->P.x > hs1->left_op->P.x);
}

inline b32
IntersectListSortI(intersect_node a, intersect_node b)
{
    if(a.pt.y == b.pt.y)
    {
        return a.pt.x < b.pt.x;
    }
    else
    {
        return a.pt.y > b.pt.y; 
    }
}

inline b32
HorzSegSorterI(horz_segment hs1, horz_segment hs2)
{
    if(!hs1.right_op || !hs2.right_op)
        return !(hs1.right_op ? 1 : 0);

    return !(hs2.left_op->P.x > hs1.left_op->P.x);
}

inline b32
LocMinSorterI(vertex *A, vertex *B)
{
    b32 Result = false;

    if(B->P.y != A->P.y)
    {
        Result = (B->P.y < A->P.y);
    }
    else
    {
        Result = (B->P.x > A->P.x);
    }

    return(!Result);
}

inline b32
IntersectListSort(intersect_node *a, intersect_node *b)
{
    //note different inequality tests ...
    return (a->pt.y == b->pt.y) ? (a->pt.x < b->pt.x) : (a->pt.y > b->pt.y);
}

inline void
InsertionSort(s32 Count, intersect_node *Nodes)
{
    for(s32 I = 1; I < Count; ++I)
    {
        intersect_node Node = Nodes[I];
        s32 J = I - 1;

        while((J >= 0) && IntersectListSortI(Nodes[J], Node))
        {
            Nodes[J + 1] = Nodes[J];
            J = J - 1;
        }

        Nodes[J + 1] = Node;
    }
}

inline void
InsertionSort(s32 Count, horz_segment *Nodes)
{
    for(s32 I = 1; I < Count; ++I)
    {
        horz_segment Node = Nodes[I];
        s32 J = I - 1;

        while((J >= 0) && HorzSegSorterI(Nodes[J], Node))
        {
            Nodes[J + 1] = Nodes[J];
            J = J - 1;
        }

        Nodes[J + 1] = Node;
    }
}

inline void
InsertionSort(s32 Count, local_minima *Nodes)
{
    for(s32 I = 1; I < Count; ++I)
    {
        local_minima Node = Nodes[I];
        s32 J = I - 1;

        while((J >= 0) && LocMinSorterI(Nodes[J].Vertex, Node.Vertex))
        {
            Nodes[J + 1] = Nodes[J];
            J = J - 1;
        }

        Nodes[J + 1] = Node;
    }
}

internal void
MergeSort(u32 Count, intersect_node *First)
{
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
        intersect_node *Temp = (intersect_node *)malloc(sizeof(intersect_node)*Count);

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

        free(Temp);
    }
}

internal void
MergeSort(u32 Count, local_minima *First)
{
    if(Count == 1)
    {
        // NOTE(casey): No work to do.
    }
    else if(Count == 2)
    {
        local_minima *EntryA = First;
        local_minima *EntryB = First + 1;
        if(LocMinSorter(EntryA, EntryB))
        {
            Swap(EntryA, EntryB);
        }
    }
    else
    {
        local_minima *Temp = (local_minima *)malloc(sizeof(local_minima)*Count);

        u32 Half0 = Count / 2;
        u32 Half1 = Count - Half0;

        Assert(Half0 >= 1);
        Assert(Half1 >= 1);

        local_minima *InHalf0 = First;
        local_minima *InHalf1 = First + Half0;
        local_minima *End = First + Count;

        MergeSort(Half0, InHalf0);
        MergeSort(Half1, InHalf1);

        local_minima *ReadHalf0 = InHalf0;
        local_minima *ReadHalf1 = InHalf1;

        local_minima *Out = Temp;
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
            else if(!LocMinSorter(ReadHalf0, ReadHalf1))
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
            
        Copy(sizeof(local_minima)*Count, Temp, First);

        free(Temp);
    }
}

internal void
MergeSort(u32 Count, horz_segment *First)
{
    if(Count == 1)
    {
        // NOTE(casey): No work to do.
    }
    else if(Count == 2)
    {
        horz_segment *EntryA = First;
        horz_segment *EntryB = First + 1;
        if(HorzSegSorter(EntryA, EntryB))
        {
            Swap(EntryA, EntryB);
        }
    }
    else
    {
        horz_segment *Temp = (horz_segment *)malloc(sizeof(horz_segment)*Count);

        u32 Half0 = Count / 2;
        u32 Half1 = Count - Half0;

        Assert(Half0 >= 1);
        Assert(Half1 >= 1);

        horz_segment *InHalf0 = First;
        horz_segment *InHalf1 = First + Half0;
        horz_segment *End = First + Count;

        MergeSort(Half0, InHalf0);
        MergeSort(Half1, InHalf1);

        horz_segment *ReadHalf0 = InHalf0;
        horz_segment *ReadHalf1 = InHalf1;

        horz_segment *Out = Temp;
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
            else if(!HorzSegSorter(ReadHalf0, ReadHalf1))
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
            
        Copy(sizeof(horz_segment)*Count, Temp, First);

        free(Temp);
    }
}

internal void
BubbleSort(u32 Count, local_minima *First)
{
    local_minima *Temp = (local_minima *)malloc(sizeof(local_minima)*Count);

    //
    // NOTE(casey): This is the O(n^2) bubble sort
    //
    for(u32 Outer = 0;
        Outer < Count;
        ++Outer)
    {
        b32 ListIsSorted = true;
        for(u32 Inner = 0;
            Inner < (Count - 1);
            ++Inner)
        {
            local_minima *EntryA = First + Inner;
            local_minima *EntryB = EntryA + 1;

            if(LocMinSorter(EntryA, EntryB))
            {
                Swap(EntryA, EntryB);
                ListIsSorted = false;
            }
        }

        if(ListIsSorted)
        {
            break;
        }
    }

    free(Temp);
}

inline b32
AngleSorter(f64 x0, f64 x1)
{
    b32 Result = false;

    if(x0 > x1)
    {
        Result = true;
    }

    return(!Result);
}

inline void
Swap(u32 *A, u32 *B)
{
    u32 Temp = *B;
    *B = *A;
    *A = Temp;
}

internal void
MergeSort(u32 Count, u32 *First, f64 *Angles)
{
    if(Count == 1)
    {
        // NOTE(casey): No work to do.
    }
    else if(Count == 2)
    {
        u32 *EntryA = First;
        u32 *EntryB = First + 1;
        if(AngleSorter(Angles[*EntryA], Angles[*EntryB]))
        {
            Swap(EntryA, EntryB);
        }
    }
    else
    {
        u32 *Temp = (u32 *)malloc(sizeof(u32)*Count);

        u32 Half0 = Count / 2;
        u32 Half1 = Count - Half0;

        Assert(Half0 >= 1);
        Assert(Half1 >= 1);

        u32 *InHalf0 = First;
        u32 *InHalf1 = First + Half0;
        u32 *End = First + Count;

        MergeSort(Half0, InHalf0, Angles);
        MergeSort(Half1, InHalf1, Angles);

        u32 *ReadHalf0 = InHalf0;
        u32 *ReadHalf1 = InHalf1;

        u32 *Out = Temp;
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
            else if(!AngleSorter(Angles[*ReadHalf0], Angles[*ReadHalf1]))
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
            
        Copy(sizeof(u32)*Count, Temp, First);

        free(Temp);
    }
}

inline u32
SortKeyToU32(r32 SortKey)
{
    // NOTE(casey): We need to turn our 32-bit floating point value
    // into some strictly ascending 32-bit unsigned integer value
    u32 Result = *(u32 *)&SortKey;
    if(Result & 0x80000000)
    {
        Result = ~Result;
    }
    else
    {
        Result |= 0x80000000;
    }

    return(Result);
}

internal void
RadixSort(u32 Count, u32 *First, u32 *Temp, f32 *Angles)
{
    u32 *Source = First;
    u32 *Dest = Temp;
    for(u32 ByteIndex = 0;
        ByteIndex < 32;
        ByteIndex += 8)
    {
        u32 SortKeyOffsets[256] = {};

        // NOTE(casey): First pass - count how many of each key
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            u32 RadixValue = SortKeyToU32(Angles[Source[Index]]);
            u32 RadixPiece = (RadixValue >> ByteIndex) & 0xFF;
            ++SortKeyOffsets[RadixPiece];
        }

        // NOTE(casey): Change counts to offsets
        u32 Total = 0;
        for(u32 SortKeyIndex = 0;
            SortKeyIndex < ArrayCount(SortKeyOffsets);
            ++SortKeyIndex)
        {
            u32 KeyCount = SortKeyOffsets[SortKeyIndex];
            SortKeyOffsets[SortKeyIndex] = Total;
            Total += KeyCount;
        }

        // NOTE(casey): Second pass - place elements into the right location
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            u32 RadixValue = SortKeyToU32(Angles[Source[Index]]);
            u32 RadixPiece = (RadixValue >> ByteIndex) & 0xFF;
            Dest[SortKeyOffsets[RadixPiece]++] = Source[Index];
        }

        u32 *SwapTemp = Dest;
        Dest = Source;
        Source = SwapTemp;
    }
}
