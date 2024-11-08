#if !defined(CLIPPER_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

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
    output_rectangle *OutRect;
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
    output_rectangle *outrec;
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

struct poly_path
{
    poly_path *Parent;
    poly_path *Children;
    path_s64 Polygon;
};

struct output_rectangle
{
    u32 Index;
    output_rectangle *Owner;
    active *FrontEdge;
    active *BackEdge;
    output_point *Points;
    poly_path *PolyPath;
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

struct clipper
{
    f64 Scale;
    f64 InvScale;

    u32 VertexListCount;
    vertex **VertexLists;//[MAX_PATH_COUNT];

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
};

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
GetNewActive(void)
{
    active *Result = MallocStruct(active);
    Result->wind_dx = 1;

    return(Result);
}

inline output_point *
GetOutPt(v2_s64 P, output_rectangle *OutRect)
{
    output_point *Result = MallocStruct(output_point);
    Result->P = P;
    Result->OutRect = OutRect;
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

inline output_rectangle *
NewOutRec(clipper *Clipper)
{
    output_rectangle *Result = Clipper->OutRecList + Clipper->OutputRectCount;
    Result->Index = Clipper->OutputRectCount++;

    return Result;
}

#define CLIPPER_H
#endif
