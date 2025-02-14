#if !defined(CLIPPER_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

#include "clipper_simd.h"
#include "clipper_platform.h"
#include "clipper_memory.h"
#include "clipper_heap.h"
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
//    output_rectangle *OutRect;

    horz_segment *Horz;

    void *Free;
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
//    output_rectangle *outrec;
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
//    output_rectangle *Owner;

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

    active *FreeList;
};

inline void
FreeActive(clipper *Clipper, active *e)
{
    ZeroStruct(*e);
    active *Free = Clipper->FreeList;
    Clipper->FreeList = e;
    e->next_in_ael = Free;
}

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

inline active *
GetNewActive(clipper *Clipper)
{
    active *Result = 0;

    if(Clipper->FreeList)
    {
        TimeBlock("List");
        Result = Clipper->FreeList;
        Clipper->FreeList = Result->next_in_ael;
        Result->next_in_ael = 0;
    }
    else
    {
        TimeBlock("Not List");
        Result = MallocStruct(active);

#if 0
        u8 *Mem = (u8 *)Malloc(sizeof(active)*10);
        active *T = (active *)Mem;

        active *Temp = 0;
        for(s32 I = 0;
            I < 10;
            ++I)
        {
            active *A = T + I;

            Temp = Clipper->FreeList;
            Clipper->FreeList = A;
            A->next_in_ael = Temp;
        }
#endif
    }

    Result->wind_dx = 1;
    
    return(Result);
}

inline output_point *
GetOutPt(v2_s64 P, u32 OutRectIndex)
{
    output_point *Result = MallocStruct(output_point);
    Result->P = P;
    Result->OutRectIndex = OutRectIndex;
    Result->Next = Result;
    Result->Prev = Result;
    Result->Free = Result;

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

#if 0
inline output_rectangle *
NewOutRec(clipper *Clipper)
{
    if(NeedIncrease(Clipper->OutputRectCount))
    {
        IncreaseOutRecList(Clipper);
    }
    
    output_rectangle *Result = Clipper->OutRecList + Clipper->OutputRectCount;
    Result->Index = Clipper->OutputRectCount++;

    return Result;
}
#endif

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


#define CLIPPER_H
#endif
