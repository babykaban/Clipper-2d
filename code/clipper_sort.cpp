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
IntersectListSort(intersect_node *a, intersect_node *b)
{
    //note different inequality tests ...
    return (a->pt.y == b->pt.y) ? (a->pt.x < b->pt.x) : (a->pt.y > b->pt.y);
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

internal void
MergeSort(u32 Count, local_minima *First)
{
    local_minima *Temp = (local_minima *)malloc(sizeof(local_minima)*Count);

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
    }

    free(Temp);
}

internal void
MergeSort(u32 Count, horz_segment *First)
{
    horz_segment *Temp = (horz_segment *)malloc(sizeof(horz_segment)*Count);

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
    }

    free(Temp);
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
