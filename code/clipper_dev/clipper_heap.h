#if !defined(CLIPPER_HEAP_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

enum heap_type
{
    HeapType_S32,
    HeapType_S64,
    HeapType_U32,
    HeapType_U64,
};

struct sort_entry
{
    union
    {
        s32 Value_S32;
        s64 Value_S64;
        u32 Value_U32;
        u64 Value_U64;
    };
};

struct heap
{
    heap_type Type;

    sort_entry *Nodes;
    u32 MaxSize;
    u32 Size;

    void *Free;
};

inline void
InitHeap(heap *Heap, u32 MaxSize, heap_type Type)
{
    Heap->Type = Type;
    Heap->MaxSize = MaxSize;
    Heap->Size = 0;
    Heap->Nodes = MallocArray(Heap->MaxSize, sort_entry);
    Heap->Free = Heap->Nodes;
}

inline void
S32RemoveAt(s32 *Array, s32 Count, s32 Index)
{
    Array[Index] = 0;
    for(s32 I = Index;
        I < Count - 1;
        ++I)
    {
        Array[I] = Array[I + 1];
    }

    Array[Count] = 0;
}

inline void
Swap(sort_entry *A, sort_entry *B)
{
    sort_entry Temp = *B;
    *B = *A;
    *A = Temp;
}

inline b32
HeapifyDownComp(heap_type HeapType, sort_entry A, sort_entry B, b32 IsMaxHeap)
{
    b32 Result = false;
    switch(HeapType)
    {
        case HeapType_S32: {Result = IsMaxHeap ? (A.Value_S32 > B.Value_S32) : (A.Value_S32 < B.Value_S32);} break;
        case HeapType_S64: {Result = IsMaxHeap ? (A.Value_S64 > B.Value_S64) : (A.Value_S64 < B.Value_S64);} break;
        case HeapType_U32: {Result = IsMaxHeap ? (A.Value_U32 > B.Value_U32) : (A.Value_U32 < B.Value_U32);} break;
        case HeapType_U64: {Result = IsMaxHeap ? (A.Value_U64 > B.Value_U64) : (A.Value_U64 < B.Value_U64);} break;
            InvalidDefaultCase;
    }

    return(Result);
}

inline b32
HeapifyUpComp(heap_type HeapType, sort_entry A, sort_entry B, b32 IsMaxHeap)
{
    b32 Result = false;
    switch(HeapType)
    {
        case HeapType_S32: {Result = IsMaxHeap ? (A.Value_S32 < B.Value_S32) : (A.Value_S32 > B.Value_S32);} break;
        case HeapType_S64: {Result = IsMaxHeap ? (A.Value_S64 < B.Value_S64) : (A.Value_S64 > B.Value_S64);} break;
        case HeapType_U32: {Result = IsMaxHeap ? (A.Value_U32 < B.Value_U32) : (A.Value_U32 > B.Value_U32);} break;
        case HeapType_U64: {Result = IsMaxHeap ? (A.Value_U64 < B.Value_U64) : (A.Value_U64 > B.Value_U64);} break;
            InvalidDefaultCase;
    }

    return(Result);
}

inline void
MinHeapifyDown(heap *Heap, u32 Index)
{
    u32 Smallest = Index;
    u32 Left = 2*Index + 1;
    u32 Right = 2*Index + 2;

    if((Left < Heap->Size) &&
       HeapifyDownComp(Heap->Type, Heap->Nodes[Left], Heap->Nodes[Smallest], false))
    {
        Smallest = Left;
    }

    if((Right < Heap->Size) &&
       HeapifyDownComp(Heap->Type, Heap->Nodes[Right], Heap->Nodes[Smallest], false))
    {
        Smallest = Right;
    }

    if(Smallest != Index)
    {
        Swap(Heap->Nodes + Index, Heap->Nodes + Smallest);
        MinHeapifyDown(Heap, Smallest);
    }
}

inline void
MaxHeapifyDown(heap *Heap, u32 Index)
{
    u32 Largest = Index;
    u32 Left = 2*Index + 1;
    u32 Right = 2*Index + 2;

    if((Left < Heap->Size) &&
       HeapifyDownComp(Heap->Type, Heap->Nodes[Left], Heap->Nodes[Largest], true))
    {
        Largest = Left;
    }

    if((Right < Heap->Size) &&
       HeapifyDownComp(Heap->Type, Heap->Nodes[Right], Heap->Nodes[Largest], true))
    {
        Largest = Right;
    }

    if(Largest != Index)
    {
        Swap(Heap->Nodes + Index, Heap->Nodes + Largest);
        MaxHeapifyDown(Heap, Largest);
    }
}

inline void
MinHeapifyUp(heap *Heap, u32 Index)
{
    u32 Parent = (Index - 1) / 2;

    if(Index &&
       HeapifyUpComp(Heap->Type, Heap->Nodes[Parent], Heap->Nodes[Index], false))
    {
        Swap(Heap->Nodes + Index, Heap->Nodes + Parent);
        MinHeapifyUp(Heap, Parent);
    }
}

inline void
MaxHeapifyUp(heap *Heap, u32 Index)
{
    u32 Parent = (Index - 1) / 2;

    if(Index &&
       HeapifyUpComp(Heap->Type, Heap->Nodes[Parent], Heap->Nodes[Index], true))
    {
        Swap(Heap->Nodes + Index, Heap->Nodes + Parent);
        MaxHeapifyUp(Heap, Parent);
    }
}

inline void
IncreaseHeapSize(heap *Heap)
{
    Heap->Nodes = ReallocArray(Heap->Nodes, Heap->MaxSize,
                               Heap->MaxSize + BASIC_ALLOCATE_COUNT, sort_entry);
    Heap->MaxSize = Heap->MaxSize + BASIC_ALLOCATE_COUNT;
}
 
inline void
MinHeapInsertNode(heap *Heap, sort_entry Key)
{
    if(Heap->Size == Heap->MaxSize)
    {
        IncreaseHeapSize(Heap);
    }

    Assert(Heap->Size != Heap->MaxSize);

    Heap->Nodes[Heap->Size] = Key;
    ++Heap->Size;
    MinHeapifyUp(Heap, Heap->Size - 1);
}

inline void
MaxHeapInsertNode(heap *Heap, sort_entry Key)
{
    if(Heap->Size == Heap->MaxSize)
    {
        IncreaseHeapSize(Heap);
    }

    Assert(Heap->Size != Heap->MaxSize);

    Heap->Nodes[Heap->Size] = Key;
    ++Heap->Size;
    MaxHeapifyUp(Heap, Heap->Size - 1);
}

inline sort_entry
MinHeapExtractNode(heap *Heap)
{
    Assert(Heap->Size > 0);

    sort_entry Result = {};
    Result = Heap->Nodes[0];
    Heap->Nodes[0] = Heap->Nodes[Heap->Size - 1];
    --Heap->Size;

    MinHeapifyDown(Heap, 0);

    return(Result);
}

inline sort_entry
MaxHeapExtractNode(heap *Heap)
{
    Assert(Heap->Size > 0);

    sort_entry Result = {};
    Result = Heap->Nodes[0];
    Heap->Nodes[0] = Heap->Nodes[Heap->Size - 1];
    --Heap->Size;

    MaxHeapifyDown(Heap, 0);

    return(Result);
}

#define CLIPPER_HEAP_H
#endif
