/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */

inline u32
GetPathsVertexCount(paths_s64 *Paths)
{
    u32 Result = 0;
    for(s32 I = 0;
        I < Paths->PathCount;
        ++I)
    {
        Result += Paths->Paths[I].Count;
    }

    return(Result);
}

internal void
AddLocMin(clipper *Clipper, vertex *vert, path_type Type, b32 IsOpen)
{
    //make sure the vertex is added only once ...
    if(!IsVertexFlagSet(vert->Flags, VertexFlag_LocalMin))
    {
        AddVertexFlag(&vert->Flags, VertexFlag_LocalMin);
        Clipper->MinimaList = ReallocArray(Clipper->MinimaList, Clipper->MinimaListCount, (Clipper->MinimaListCount + 1), local_minima);
        InitLocalMinima(Clipper->MinimaList + Clipper->MinimaListCount, vert, Type, IsOpen);
        Clipper->MinimaListCount++;
    }
}

internal void
AddPathsInternal(clipper *Clipper, paths_s64 *Paths, path_type Type, b32 IsOpen)
{
    u32 TotalVertexCout = GetPathsVertexCount(Paths);

    if(TotalVertexCout == 0)
        return;

    Clipper->VertexLists = ReallocArray(Clipper->VertexLists,
                                        Clipper->VertexListCount,
                                        (Clipper->VertexListCount + 1), vertex *);
    ++Clipper->VertexListCount;

    Clipper->VertexLists[Clipper->VertexListCount - 1] = MallocArray(TotalVertexCout, vertex);
    vertex *Vertices = Clipper->VertexLists[Clipper->VertexListCount - 1];
    vertex *VIter = Vertices;

    for(s32 PathIndex = 0;
        PathIndex < Paths->PathCount;
        ++PathIndex)
    {
        path_s64 *Path = Paths->Paths + PathIndex;
        vertex *VIter0 = VIter;
        vertex *curr_v = VIter;
        vertex *prev_v = 0;

        if(!Path)
            continue;

        VIter->Prev = 0;
        s32 Count = 0;

        for(s32 PointIndex = 0;
            PointIndex < Path->Count;
            ++PointIndex)
        {
            v2_s64 Point = Path->Points[PointIndex];
            if(prev_v)
            {
                if(PointsAreEqual(prev_v->P, Point))
                    continue; // ie skips duplicates
                prev_v->Next = curr_v;
            }

            curr_v->Prev = prev_v;
            curr_v->P = Point;
            curr_v->Flags = 0;
            prev_v = curr_v++;
            Count++;
        }

        if(!prev_v || !prev_v->Prev)
            continue;

        if(!IsOpen && PointsAreEqual(prev_v->P, VIter0->P))
            prev_v = prev_v->Prev;

        prev_v->Next = VIter0;
        VIter0->Prev = prev_v;
        VIter = curr_v; // ie get ready for next path

        if((Count < 2) || ((Count == 2) && !IsOpen))
            continue;
        
        //now find and assign local minima
        b32 going_up = false;
        b32 going_up0 = false;
        if(IsOpen)
        {
            curr_v = VIter0->Next;
            while((curr_v != VIter0) && (curr_v->P.y == VIter0->P.y))
                curr_v = curr_v->Next;

            going_up = (curr_v->P.y <= VIter0->P.y);
            if(going_up)
            {
                VIter0->Flags = VertexFlag_OpenStart;
                AddLocMin(Clipper, VIter, Type, true);
            }
            else
            {
                VIter0->Flags = VertexFlag_OpenStart | VertexFlag_LocalMax;
            }
        }
        else // closed path
        {
            prev_v = VIter0->Prev;
            while((prev_v != VIter0) && (prev_v->P.y == VIter0->P.y))
                prev_v = prev_v->Prev;

            if(prev_v == VIter0)
                continue; // only open paths can be completely flat

            going_up = (prev_v->P.y > VIter0->P.y);
        }

        going_up0 = going_up;
        prev_v = VIter0;
        curr_v = VIter0->Next;
        while (curr_v != VIter0)
        {
            if((curr_v->P.y > prev_v->P.y) && going_up)
            {
                AddVertexFlag(&prev_v->Flags, VertexFlag_LocalMax);
                going_up = false;
            }
            else if((curr_v->P.y < prev_v->P.y) && !going_up)
            {
                going_up = true;
                AddLocMin(Clipper, prev_v, Type, IsOpen);
            }

            prev_v = curr_v;
            curr_v = curr_v->Next;
        }

        if(IsOpen)
        {
            AddVertexFlag(&prev_v->Flags, VertexFlag_OpenEnd);
            if(going_up)
                AddVertexFlag(&prev_v->Flags, VertexFlag_LocalMax);
            else
                AddLocMin(Clipper, prev_v, Type, IsOpen);
        }
        else if(going_up != going_up0)
        {
            if (going_up0)
                AddLocMin(Clipper, prev_v, Type, false);
            else
                AddVertexFlag(&prev_v->Flags, VertexFlag_LocalMax);
        }
    }
}

inline void
AddPaths(clipper *Clipper, paths_s64 *Paths, path_type Type, b32 IsOpen)
{
    if(IsOpen)
        Clipper->HasOpenPaths = true;
    Clipper->MinimaListSorted = false;

    AddPathsInternal(Clipper, Paths, Type, IsOpen);
}

inline void
AddSubjects(clipper *Clipper, paths_f64 *Subjects)
{
    paths_s64 Paths = ScalePathsF64(Subjects, Clipper->Scale);
    AddPaths(Clipper, &Paths, PathType_Subject, false);
}

inline void
AddClips(clipper *Clipper, paths_f64 *Clips)
{
    paths_s64 Paths = ScalePathsF64(Clips, Clipper->Scale);
    AddPaths(Clipper, &Paths, PathType_Clip, false);
}

inline void
InitClipper(clipper *Clipper, s32 Precision)
{
    Clipper->Scale = pow((f64)FLT_RADIX, ilogb(pow(10, Precision)) + 1);
    Clipper->InvScale = 1.0 / Clipper->Scale;
    InitHeap(&Clipper->ScanLineMaxHeap, 256, HeapType_S64);
    Clipper->OutRecList = MallocArray(512, output_rectangle);
    Clipper->HorzSegList = MallocArray(512, horz_segment);
    Clipper->IntersectNodes = MallocArray(512, intersect_node);
}

inline paths_s64
BooleanOp(clip_type ClipType, fill_rule FillRule,
          paths_s64 *Subjects, paths_s64 *Clips, s32 Precision = 2)
{
    CheckPrecisionRange(&Precision);

    paths_s64 Result = {};

    clipper Clipper = {};

//    InitClipper(&Clipper, Precision);
//    AddSubjects(&Clipper, Subjects);
//    AddClips(&Clipper, Clips);

//    Execute(&Clipper, ClipType, FillRule, &Result, 0);

    return(Result);
}
