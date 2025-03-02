#if !defined(CLIPPER_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

#define CLIPPER_AVX512 0
#define CLIPPER_AVX2 0
#define CLIPPER_AVX 0
#define CLIPPER_SSE 0

// https://github.com/AngusJohnson/Clipper2/discussions/334
// #discussioncomment-4248602
#if defined(_MSC_VER) && ( defined(_M_AMD64) || defined(_M_X64) )
#include <xmmintrin.h>
#include <emmintrin.h>
#define fmin(a,b) _mm_cvtsd_f64(_mm_min_sd(_mm_set_sd(a),_mm_set_sd(b)))
#define fmax(a,b) _mm_cvtsd_f64(_mm_max_sd(_mm_set_sd(a),_mm_set_sd(b)))
#define nearbyint(a) _mm_cvtsd_si64(_mm_set_sd(a)) /* Note: expression type is (int64_t) */

#if defined(_AVX512)
#define CLIPPER_AVX512 1
#elif defined(_AVX2)
#define CLIPPER_AVX2 1
#elif defined(_AVX)
#define CLIPPER_AVX 1
#elif defined(_M_IX86_FP) && _M_IX86_FP >= 2
#define CLIPPER_SSE 1
#endif
#endif

#include "clipper_core.h"

enum clip_type
{
    ClipType_NoClip,

    ClipType_Intersection,
    ClipType_Union,
    ClipType_Difference,
    ClipType_Xor,

    ClipType_Count,
};

enum fill_rule
{
    FillRule_EvenOdd,
    FillRule_NonZero,
    FillRule_Positive,
    FillRule_Negative,

    FillRule_Count,
};

enum path_type
{
    PathType_Subject,
    PathType_Clip,
};

enum join_with
{
    JoinWith_NoJoin,
    JoinWith_Left,
    JoinWith_Right,
};

enum vertex_flag
{
    VertexFlag_Empty = 0,
    VertexFlag_OpenStart = (1 << 0),
    VertexFlag_OpenEnd = (1 << 1),
    VertexFlag_LocalMax = (1 << 2),
    VertexFlag_LocalMin = (1 << 3),
};

inline void
AddVertexFlag(u32 *Flags, u32 Flag)
{
    *Flags = (*Flags) | Flag;
}

inline void
ClearVertexFlag(u32 *Flags, u32 Flag)
{
    *Flags = (*Flags) & ~Flag;
}

inline b32
IsVertexFlagSet(u32 Flags, u32 Flag)
{
    b32 Result = Flags & Flag;

    return(Result);
}

struct vertex
{
    v2_s64 P;
    vertex *Next;
    vertex *Prev;

    u32 Flags;
};

struct output_rectangle;
struct horz_segment;
struct output_point
{
    v2_s64 P;
    output_point *Next;
    output_point *Prev;

    u32 OutRectIndex;

    horz_segment *Horz;
};

struct horz_segment
{
    output_point *left_op;
    output_point *right_op;
    b32 left_to_right;
};

struct horz_join
{
    output_point *op1;
    output_point *op2;
};

struct local_minima
{
    vertex *Vertex;
    path_type PolyType;
    b32 IsOpen;
};

struct active
{
    v2_s64 bot;
    v2_s64 top;
    s64 curr_x;     //current (updated at every new scanline)
    f64 dx;
    s32 wind_dx;            //1 or -1 depending on winding direction
    s32 wind_cnt;
    s32 wind_cnt2;      //winding count of the opposite polytype

    u32 outrecIndex;
    //AEL: 'active edge list' (Vatti's AET - active edge table)
    //     a linked list of all edges (from left to right) that are present
    //     (or 'active') within the current scanbeam (a horizontal 'beam' that
    //     sweeps from bottom to top over the paths in the clipping operation).
    active *prev_in_ael;
    active *next_in_ael;
    //SEL: 'sorted edge list' (Vatti's ST - sorted table)
    //     linked list used when sorting edges into their new positions at the
    //     top of scanbeams, but also (re)used to process horizontals.
    active *prev_in_sel;
    active *next_in_sel;
    active *jump;
    vertex *vertex_top;
    local_minima *local_min;  // the bottom of an edge 'bound' (also Vatti)
    b32 IsLeftBound;
    join_with JoinWith;
};

struct output_rectangle
{
    u32 Index;
    u32 OwnerIndex;

    active *FrontEdge;
    active *BackEdge;
    output_point *Points;
    u32 SplitCount;
    output_rectangle *Splits;
    output_rectangle *RecursiveSplit;

    rectangle2i Bounds;
    path_s64 Path;
    b32 IsOpen;
};

struct intersect_node
{
    v2_s64 pt;
    active *edge1;
    active *edge2;
};

struct vertex_list
{
    u32 VertexCount;
    vertex *Vertices;
};

struct clipper
{
    f64 Scale;
    f64 InvScale;

    u32 VertexListCount;
    vertex_list *VertexLists;

    u32 MinimaListCount;
    local_minima *MinimaList;       //pointers in case of memory reallocs
    b32 MinimaListSorted;

    clip_type ClipType;
    fill_rule FillRule;

    local_minima *CurrentLocalMinima;
    heap ScanLineMaxHeap;
    active *ActiveEdgeList;
    active *StoredEdgeList;
    s64 BottomY;

    u32 OutputRectCount;
    output_rectangle *OutRecList; //pointers in case list memory reallocated

    u32 HorzCount;
    horz_segment *HorzSegList;

    u32 JointCount;
    horz_join *HorzJoinList;
    
    u32 IntersectNodeCount;
    intersect_node *IntersectNodes;

    b32 PreserveCollinear;
    b32 ReverseSolution;
    b32 HasOpenPaths;
    b32 Succeeded;

    active *ActivesFreeList;
    u32 DeallocActivesCount;
    void **DeallocActivesList;

    output_point *OutPtFreeList;
    u32 DeallocOutPtsCount;
    void **DeallocOutPtsList;

};

inline void
IncreaseOutRecList(clipper *Clipper)
{
    Clipper->OutRecList =
        ReallocArray(Clipper->OutRecList, Clipper->OutputRectCount,
                     Clipper->OutputRectCount + BASIC_ALLOCATE_COUNT, output_rectangle);

#if RECORD_MEMORY_USEAGE
    ArrayMaxSizes[ArrayType_OutRec] = Clipper->OutputRectCount + BASIC_ALLOCATE_COUNT;
#endif
}

inline void
IncreaseHorzSegList(clipper *Clipper)
{
    Clipper->HorzSegList =
        ReallocArray(Clipper->HorzSegList, Clipper->HorzCount,
                     Clipper->HorzCount + BASIC_ALLOCATE_COUNT, horz_segment);

#if RECORD_MEMORY_USEAGE
    ArrayMaxSizes[ArrayType_HorzSegList] = Clipper->HorzCount + BASIC_ALLOCATE_COUNT;
#endif
}

inline void
IncreaseHorzJoinList(clipper *Clipper)
{
    Clipper->HorzJoinList =
        ReallocArray(Clipper->HorzJoinList, Clipper->JointCount,
                     Clipper->JointCount + BASIC_ALLOCATE_COUNT, horz_join);

#if RECORD_MEMORY_USEAGE
    ArrayMaxSizes[ArrayType_HorzJoinList] = Clipper->JointCount + BASIC_ALLOCATE_COUNT;
#endif
}

inline void
IncreaseIntersectNodes(clipper *Clipper)
{
    Clipper->IntersectNodes =
        ReallocArray(Clipper->IntersectNodes, Clipper->IntersectNodeCount,
                     Clipper->IntersectNodeCount + BASIC_ALLOCATE_COUNT, intersect_node);

#if RECORD_MEMORY_USEAGE
    ArrayMaxSizes[ArrayType_IntersectNode] = Clipper->IntersectNodeCount + BASIC_ALLOCATE_COUNT;
#endif
}

inline void
IncreaseMinimaList(clipper *Clipper)
{
    Clipper->MinimaList =
        ReallocArray(Clipper->MinimaList, Clipper->MinimaListCount,
                     Clipper->MinimaListCount + BASIC_ALLOCATE_COUNT, local_minima);

#if RECORD_MEMORY_USEAGE
    ArrayMaxSizes[ArrayType_MinimaList] = Clipper->MinimaListCount + BASIC_ALLOCATE_COUNT;
#endif
}

inline void
IncreaseVertexLists(clipper *Clipper)
{
    Clipper->VertexLists =
        ReallocArray(Clipper->VertexLists, Clipper->VertexListCount,
                     Clipper->VertexListCount + BASIC_ALLOCATE_COUNT, vertex_list);

#if RECORD_MEMORY_USEAGE
    ArrayMaxSizes[ArrayType_VertexLists] = Clipper->VertexListCount + BASIC_ALLOCATE_COUNT;
#endif
}

inline void
IncreaseDeallocateActivesList(clipper *Clipper)
{
    Clipper->DeallocActivesList =
        ReallocArray(Clipper->DeallocActivesList, Clipper->DeallocActivesCount,
                     Clipper->DeallocActivesCount + BASIC_ALLOCATE_COUNT, void *);

#if RECORD_MEMORY_USEAGE
    ArrayMaxSizes[ArrayType_DeallocateActivesList] =
        Clipper->DeallocActivesCount + BASIC_ALLOCATE_COUNT;
#endif
}

inline void
IncreaseDeallocateOutPtsList(clipper *Clipper)
{
    Clipper->DeallocOutPtsList =
        ReallocArray(Clipper->DeallocOutPtsList, Clipper->DeallocOutPtsCount,
                     Clipper->DeallocOutPtsCount + BASIC_ALLOCATE_COUNT, void *);

#if RECORD_MEMORY_USEAGE
    ArrayMaxSizes[ArrayType_DeallocateOutPtsList] =
        Clipper->DeallocOutPtsCount + BASIC_ALLOCATE_COUNT;
#endif
}

inline horz_join
HorzJoin(output_point *op1, output_point *op2)
{
    horz_join Result = {};
    Result.op1 = op1;
    Result.op2 = op2;

    return(Result);
}

inline horz_segment *
GetHorzSegment(output_point *Point)
{
    horz_segment *Result = MallocStruct(horz_segment);
    Result->left_to_right = true;
    Result->left_op = Point;

    return(Result);
}

inline void
InitLocalMinima(local_minima *Minima, vertex *Vertex, path_type Type, b32 Open)
{
    Minima->Vertex = Vertex;
    Minima->PolyType = Type;
    Minima->IsOpen = Open;
}

inline void
InitActive(active *Active)
{
    ZeroStruct(*Active);
    Active->wind_dx = 1;
}

inline void
FreeActive(clipper *Clipper, active *e)
{
    ZeroStruct(*e);

    active *Free = Clipper->ActivesFreeList;
    Clipper->ActivesFreeList = e;
    e->next_in_ael = Free;
}

inline active *
GetNewActive(clipper *Clipper)
{
    active *Result = 0;

    if(Clipper->ActivesFreeList)
    {
        Result = Clipper->ActivesFreeList;
        Clipper->ActivesFreeList = Result->next_in_ael;
        Result->next_in_ael = 0;
    }
    else
    {
        active *Mem = (active *)Malloc(sizeof(active)*BASIC_ALLOCATE_COUNT);
        Clipper->DeallocActivesList[Clipper->DeallocActivesCount++] = Mem;
        Assert(Clipper->DeallocActivesCount < BASIC_ALLOCATE_COUNT);
        
        for(s32 I = 0;
            I < BASIC_ALLOCATE_COUNT;
            ++I)
        {
            active *New = Mem + I;
            active *Free = Clipper->ActivesFreeList;
            Clipper->ActivesFreeList = New;
            New->next_in_ael = Free;
        }

        Result = Clipper->ActivesFreeList;
        Clipper->ActivesFreeList = Result->next_in_ael;
        Result->next_in_ael = 0;
    }

    Result->wind_dx = 1;
    
    return(Result);
}

inline void
FreeOutPt(clipper *Clipper, output_point *p)
{
    ZeroStruct(*p);

    output_point *Free = Clipper->OutPtFreeList;
    Clipper->OutPtFreeList = p;
    p->Next = Free;
}

inline output_point *
GetOutPt(clipper *Clipper, v2_s64 P, u32 OutRectIndex)
{

    output_point *Result = 0;
    if(Clipper->OutPtFreeList)
    {
        Result = Clipper->OutPtFreeList;
        Clipper->OutPtFreeList = Result->Next;
        Result->Next = 0;
    }
    else
    {
        output_point *Mem = MallocArray(BASIC_ALLOCATE_COUNT, output_point);
        Clipper->DeallocOutPtsList[Clipper->DeallocOutPtsCount++] = Mem;
        Assert(Clipper->DeallocOutPtsCount < BASIC_ALLOCATE_COUNT);
        
        for(s32 I = 0;
            I < BASIC_ALLOCATE_COUNT;
            ++I)
        {
            output_point *New = Mem + I;
            output_point *Free = Clipper->OutPtFreeList;
            Clipper->OutPtFreeList = New;
            New->Next = Free;
        }

        Result = Clipper->OutPtFreeList;
        Clipper->OutPtFreeList = Result->Next;
        Result->Next = 0;
    }

    Result->P = P;
    Result->OutRectIndex = OutRectIndex;
    Result->Next = Result;
    Result->Prev = Result;

    return(Result);
}

inline intersect_node
IntersectNode(active *edge1, active *edge2, v2_s64 pt)
{
    intersect_node Result = {};
    Result.pt = pt;
    Result.edge1 = edge1;
    Result.edge2 = edge2;

    return(Result);
}

inline u32
NewOutRec(clipper *Clipper)
{
    if(NeedIncrease(Clipper->OutputRectCount))
    {
        IncreaseOutRecList(Clipper);
    }
    
    u32 Result = Clipper->OutputRectCount;
    output_rectangle *Outrec = Clipper->OutRecList + Clipper->OutputRectCount;
    Outrec->Index = Clipper->OutputRectCount++;

    return(Result);
}

inline output_rectangle *
GetOutRec(clipper *Clipper, u32 Index)
{
    output_rectangle *Result = Clipper->OutRecList + Index;
    return(Result);
}

inline void
FreePaths(paths_f64 *Paths)
{
    for(s32 I = 0;
        I < Paths->PathCount;
        ++I)
    {
        path_f64 *Path = Paths->Paths + I;
#if RECORD_MEMORY_USEAGE
        Free(Path->Points, sizeof(v2_f64)*Path->AllocateCount);
        PathMemoryUsed -= sizeof(v2_f64)*Path->AllocateCount;
#else
        Free(Path->Points, sizeof(v2_f64)*Path->AllocateCount);
#endif
    }

#if RECORD_MEMORY_USEAGE
    Free(Paths->Paths, Paths->AllocateCount*sizeof(path_f64));
    PathMemoryUsed -= Paths->AllocateCount*sizeof(path_f64);
#else
    Free(Paths->Paths, Paths->AllocateCount*sizeof(path_s64));
#endif
}

inline void
FreePaths(paths_s64 *Paths)
{
    for(s32 I = 0;
        I < Paths->PathCount;
        ++I)
    {
        path_s64 *Path = Paths->Paths + I;
#if RECORD_MEMORY_USEAGE
        Free(Path->Points, sizeof(v2_s64)*Path->AllocateCount);
        PathMemoryUsed -= sizeof(v2_s64)*Path->AllocateCount;
#else
        Free(Path->Points, sizeof(v2_s64)*Path->AllocateCount);
#endif
    }

#if RECORD_MEMORY_USEAGE
    Free(Paths->Paths, Paths->AllocateCount*sizeof(path_s64));
    PathMemoryUsed -= Paths->AllocateCount*sizeof(path_s64);
#else
    Free(Paths->Paths, Paths->AllocateCount*sizeof(path_s64));
#endif
}

// NOTE(babykaban): SORTS ====================================================================================================================================
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
// ===========================================================================================================================================================

#define CLIPPER_H
#endif
