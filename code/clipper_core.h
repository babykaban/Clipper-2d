#if !defined(CLIPPER_CORE_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

#include "clipper_math.h"
#include "clipper_math_f64.h"
#include "clipper_math_s64.h"

#ifndef PI
  global_variable f64 PI = 3.141592653589793238;
#endif

#ifdef CLIPPER2_MAX_DECIMAL_PRECISION
  global_variable s32 CLIPPER2_MAX_DEC_PRECISION = CLIPPER2_MAX_DECIMAL_PRECISION;
#else
  global_variable s32 CLIPPER2_MAX_DEC_PRECISION = 8; // see Discussions #564
#endif

#define MAX_COORD (INT64_MAX >> 2)
#define MIN_COORD -MAX_COORD
#define INVALID = INT64_MAX;
#define MAX_COORD_F64 (double)(MAX_COORD);
#define MIN_COORD_F64 (double)(MIN_COORD);

#define F64Max MAX_DBL
#define F64Min MIN_DBL

#define InvalidPointS64 PointS64(INT64_MAX, INT64_MAX)
#define InvalidPointF64 PointF64(DBL_MAX, DBL_MAX)

#define InvalidPointRectF64 InvertedInfinityRectangle2()

inline path_f64
GetPathF64(s32 Count)
{
    path_f64 Result = {};
    Result.AllocateCount = Count;
    Result.Points = MallocArray(Count, v2_f64);

#if RECORD_MEMORY_USEAGE
    PathMemoryUsed += Count*sizeof(v2_f64);
#endif
    
    return(Result);
}

inline paths_f64
GetPathsF64(s32 Count)
{
    paths_f64 Result = {};
    Result.AllocateCount = Count;
    Result.Paths = MallocArray(Count, path_f64);
    
#if RECORD_MEMORY_USEAGE
    PathMemoryUsed += Count*sizeof(path_f64);
#endif

    return(Result);
}

inline paths_f64
GetAndCopyPathsF64(paths_f64 *Old, s32 Count)
{
    paths_f64 Result = {};
    Result.AllocateCount = Count;
    Result.Paths = MallocArray(Count, path_f64);

    Result.PathCount = Old->PathCount;
    Copy(Old->PathCount*sizeof(path_f64), Old->Paths, Result.Paths);
    
#if RECORD_MEMORY_USEAGE
    PathMemoryUsed += Count*sizeof(path_f64);
#endif

    return(Result);
}

inline path_s64
GetPathS64(s32 Count)
{
    path_s64 Result = {};
    Result.AllocateCount = Count;
    Result.Points = MallocArray(Count, v2_s64);

#if RECORD_MEMORY_USEAGE
    PathMemoryUsed += Count*sizeof(v2_s64);
#endif
    
    return(Result);
}

inline paths_s64
GetPathsS64(s32 Count)
{
    paths_s64 Result = {};
    Result.AllocateCount = Count;
    Result.Paths = MallocArray(Count, path_s64);

#if RECORD_MEMORY_USEAGE
    PathMemoryUsed += Count*sizeof(path_s64);
#endif
    
    return(Result);
}

inline void
IncreasePathF64(path_f64 *Path)
{
    Path->Points =
        ReallocArray(Path->Points, Path->Count,
                     Path->Count + BASIC_ALLOCATE_COUNT, v2_f64);
    Path->AllocateCount = Path->Count + BASIC_ALLOCATE_COUNT;

    PathMemoryUsed += BASIC_ALLOCATE_COUNT*sizeof(v2_f64);
}

inline void
IncreasePathS64(path_s64 *Path)
{
    Path->Points =
        ReallocArray(Path->Points, Path->Count,
                     Path->Count + BASIC_ALLOCATE_COUNT, v2_s64);
    Path->AllocateCount = Path->Count + BASIC_ALLOCATE_COUNT;
    PathMemoryUsed += BASIC_ALLOCATE_COUNT*sizeof(v2_f64);
}

inline void
CheckPrecisionRange(s32 *Precision)
{
    if(!((*Precision >= -CLIPPER2_MAX_DEC_PRECISION) &&
         (*Precision <= CLIPPER2_MAX_DEC_PRECISION)))
    {
        *Precision = (*Precision > 0) ? CLIPPER2_MAX_DEC_PRECISION : -CLIPPER2_MAX_DEC_PRECISION;
    }
}

inline path_f64
RectAsPath(rectangle2 Rect)
{
    path_f64 Result = GetPathF64(4);
    Result.Points[0] = V2F64(Rect.Min.x, Rect.Max.y);
    Result.Points[1] = Rect.Max;
    Result.Points[2] = V2F64(Rect.Max.x, Rect.Min.y);
    Result.Points[3] = Rect.Min;

    return(Result);
}

inline path_f64
ScalePath(path_f64 *Source, v2_f64 Scale)
{
    // TODO(babykaban): Error Handling

    path_f64 Result = GetPathF64(Source->Count);
    Result.Count = Source->Count;
    if((Scale.x == 0) || (Scale.y == 0))
    {
//        error_code |= scale_error_i;
//        DoError(scale_error_i);
        // if no exception, treat as non-fatal error
        if(Scale.x == 0)
            Scale.x = 1.0;
        if(Scale.y == 0)
            Scale.y = 1.0;
    }
    
    for(s32 I = 0;
        I < Source->Count;
        ++I)
    {
        Result.Points[I] = Scale*Source->Points[I];
    }

    return(Result);
}

inline void
ScaleCurrentPath(path_f64 *Source, v2_f64 Scale)
{
    // TODO(babykaban): Error Handling
    if((Scale.x == 0) || (Scale.y == 0))
    {
//        error_code |= scale_error_i;
//        DoError(scale_error_i);
        // if no exception, treat as non-fatal error
        if(Scale.x == 0)
            Scale.x = 1.0;
        if(Scale.y == 0)
            Scale.y = 1.0;
    }
    
    for(s32 I = 0;
        I < Source->Count;
        ++I)
    {
        Source->Points[I] = Scale*Source->Points[I];
    }
}

inline path_f64
ScaleConvertPathToF64(path_s64 *Source, v2_f64 Scale)
{
    // TODO(babykaban): Error Handling

    path_f64 Result = GetPathF64(Source->Count);
    Result.Count = Source->Count;
    if((Scale.x == 0) || (Scale.y == 0))
    {
//        error_code |= scale_error_i;
//        DoError(scale_error_i);
        // if no exception, treat as non-fatal error
        if(Scale.x == 0)
            Scale.x = 1.0;
        if(Scale.y == 0)
            Scale.y = 1.0;
    }
    
    for(s32 I = 0;
        I < Source->Count;
        ++I)
    {
        Result.Points[I] = Scale*V2F64(Source->Points[I]);
    }

    return(Result);
}

inline path_s64
ScaleConvertPathToS64(path_f64 *Source, v2_f64 Scale)
{
    // TODO(babykaban): Error Handling

    path_s64 Result = GetPathS64(Source->Count);
    Result.Count = Source->Count;
    if((Scale.x == 0) || (Scale.y == 0))
    {
//        error_code |= scale_error_i;
//        DoError(scale_error_i);
        // if no exception, treat as non-fatal error
        if(Scale.x == 0)
            Scale.x = 1.0;
        if(Scale.y == 0)
            Scale.y = 1.0;
    }
    
    for(s32 I = 0;
        I < Source->Count;
        ++I)
    {
        Result.Points[I] = V2S64(Scale*Source->Points[I]);
    }

    return(Result);
}

inline path_f64
ScalePath(path_f64 *Source, f64 Scale)
{
    path_f64 Result = ScalePath(Source, V2F64(Scale, Scale));
    return(Result);
}

inline void
ScaleCurrentPath(path_f64 *Source, f64 Scale)
{
    ScaleCurrentPath(Source, V2F64(Scale, Scale));
}

inline path_s64
ScaleConvertPathToS64(path_f64 *Source, f64 Scale)
{
    path_s64 Result = ScaleConvertPathToS64(Source, V2F64(Scale, Scale));
    return(Result);
}

inline path_f64
ScaleConvertPathToF64(path_s64 *Source, f64 Scale)
{
    path_f64 Result = ScaleConvertPathToF64(Source, V2F64(Scale, Scale));
    return(Result);
}

inline paths_f64
ScalePaths(paths_f64 *Source, f64 ScaleX, f64 ScaleY)
{
    paths_f64 Result = GetPathsF64(Source->PathCount);
    Result.PathCount = Source->PathCount;
    
    for(s32 I = 0;
        I < Source->PathCount;
        ++I)
    {
        Result.Paths[I] = ScalePath(Source->Paths + I, V2F64(ScaleX, ScaleY));
    }

    return(Result);
}

inline void
ScaleCurrentPaths(paths_f64 *Source, f64 ScaleX, f64 ScaleY)
{
    for(s32 I = 0;
        I < Source->PathCount;
        ++I)
    {
        ScaleCurrentPath(Source->Paths + I, V2F64(ScaleX, ScaleY));
    }
}

inline paths_f64
ScaleConvertPathsToF64(paths_s64 *Source, f64 ScaleX, f64 ScaleY)
{
    paths_f64 Result = GetPathsF64(Source->PathCount);
    Result.PathCount = Source->PathCount;
    
    for(s32 I = 0;
        I < Source->PathCount;
        ++I)
    {
        Result.Paths[I] = ScaleConvertPathToF64(Source->Paths + I, V2F64(ScaleX, ScaleY));
    }

    return(Result);
}

inline paths_s64
ScaleConvertPathsToS64(paths_f64 *Source, f64 ScaleX, f64 ScaleY)
{
    paths_s64 Result = GetPathsS64(Source->PathCount);
    Result.PathCount = Source->PathCount;
    
    for(s32 I = 0;
        I < Source->PathCount;
        ++I)
    {
        Result.Paths[I] = ScaleConvertPathToS64(Source->Paths + I, V2F64(ScaleX, ScaleY));
    }

    return(Result);
}

inline paths_f64
ScalePaths(paths_f64 *Source, f64 Scale)
{
    paths_f64 Result = ScalePaths(Source, Scale, Scale);
    return(Result);
}

inline void
ScaleCurrentPaths(paths_f64 *Source, f64 Scale)
{
    ScaleCurrentPaths(Source, Scale, Scale);
}

inline paths_s64
ScaleConvertPathsToS64(paths_f64 *Source, f64 Scale)
{
    paths_s64 Result = ScaleConvertPathsToS64(Source, Scale, Scale);
    return(Result);
}

inline paths_f64
ScaleConvertPathsToS64(paths_s64 *Source, f64 Scale)
{
    paths_f64 Result = ScaleConvertPathsToF64(Source, Scale, Scale);
    return(Result);
}

inline path_s64
ConvertF64PathToS64(path_f64 *Source)
{
    path_s64 Result = GetPathS64(Source->Count);
    Result.Count = Source->Count;
    for(s32 I = 0;
        I < Source->Count;
        ++I)
    {
        Result.Points[I] = V2S64(Source->Points[I]);
    }

    return(Result);
}

inline paths_s64
ConvertF64PathsToS64(paths_f64 *Source)
{
    paths_s64 Result = GetPathsS64(Source->PathCount);
    Result.PathCount = Source->PathCount;

    for(s32 I = 0;
        I < Source->PathCount;
        ++I)
    {
        Result.Paths[I] = ConvertF64PathToS64(Source->Paths + I);
    }

    return(Result);
}

inline path_f64
StripNearEqual(path_f64 *Path, f64 MaxDistSq, b32 IsClosedPath)
{
    path_f64 Result = {};
    if(Path->Count > 0)
    {
        Result = GetPathF64(Path->Count);

        v2_f64 *PathIter = Path->Points;
        v2_f64 *PathEnd = Path->Points + (Path->Count - 1);

        v2_f64 FirstP = *PathIter++;
        v2_f64 LastP = FirstP;
        Result.Points[Result.Count++] = *PathIter;

        for(; !PointsAreEqual(*PathIter, *PathEnd); ++PathIter)
        {
            if(!NearEqual(*PathIter, LastP, MaxDistSq))
            {
                LastP = *PathIter;
                Result.Points[Result.Count++] = LastP;
            }
        }

        if(IsClosedPath)
        {
            while((Result.Count > 1) &&
                  NearEqual(Result.Points[Result.Count - 1], FirstP, MaxDistSq))
            {
                Result.Points[--Result.Count] = {};
            }

            Result.Points = (v2_f64 *)realloc(Result.Points, sizeof(v2_f64)*Result.Count);
        }
    }

    return(Result);
}

inline paths_f64
StripNearEqual(paths_f64 *Source, f64 MaxDistSq, b32 IsClosedPath)
{
    paths_f64 Result = GetPathsF64(Source->PathCount);

    for(s32 I = 0;
        I < Source->PathCount;
        ++I)
    {
        Result.Paths[I] = StripNearEqual(Source->Paths + I, MaxDistSq, IsClosedPath);
    }

    return(Result);
}

inline void
StripDuplicates(path_f64 *Path, b32 IsClosedPath)
{
    for(s32 I = 0;
        I < (Path->Count - 1);
        ++I)
    {
        if(PointsAreEqual(Path->Points[I], Path->Points[I + 1]))
        {
            for(s32 J = I;
                J < (Path->Count - 1);
                ++J)
            {
                Path->Points[J] = Path->Points[J + 1];
            }

            --Path->Count;
            --I;
        }
    }

    if(IsClosedPath)
    {
        while((Path->Count > 1) &&
              PointsAreEqual(Path->Points[Path->Count - 1], Path->Points[0]))
        {
            Path->Points[--Path->Count] = {};
        }
    }

    Path->Points = (v2_f64 *)realloc(Path->Points, sizeof(v2_f64)*Path->Count);
}

inline void
StripDuplicates(paths_f64 *Source, b32 IsClosedPath)
{
    for(s32 I = 0;
        I < Source->PathCount;
        ++I)
    {
        StripDuplicates(Source->Paths + I, IsClosedPath);
    }
}

inline path_s64
RectAsPath(rectangle2i Rect)
{
    path_s64 Result = GetPathS64(4);
    Result.Points[0] = V2S64(Rect.Min.x, Rect.Max.y);
    Result.Points[1] = Rect.Max;
    Result.Points[2] = V2S64(Rect.Max.x, Rect.Min.y);
    Result.Points[3] = Rect.Min;

    return(Result);
}

inline path_f64
ConvertS64PathToF64(path_s64 *Source)
{
    path_f64 Result = GetPathF64(Source->Count);
    Result.Count = Source->Count;
    for(s32 I = 0;
        I < Source->Count;
        ++I)
    {
        Result.Points[I] = V2F64(Source->Points[I]);
    }

    return(Result);
}

inline paths_f64
ConvertS64PathsToF64(paths_s64 *Source)
{
    paths_f64 Result = GetPathsF64(Source->PathCount);
    Result.PathCount = Source->PathCount;

    for(s32 I = 0;
        I < Source->PathCount;
        ++I)
    {
        Result.Paths[I] = ConvertS64PathToF64(Source->Paths + I);
    }

    return(Result);
}

inline path_s64
StripNearEqual(path_s64 *Path, f64 MaxDistSq, b32 IsClosedPath)
{
    path_s64 Result = {};
    if(Path->Count > 0)
    {
        Result = GetPathS64(Path->Count);

        v2_s64 *PathIter = Path->Points;
        v2_s64 *PathEnd = Path->Points + (Path->Count - 1);

        v2_s64 FirstP = *PathIter++;
        v2_s64 LastP = FirstP;
        Result.Points[Result.Count++] = *PathIter;

        for(; !PointsAreEqual(*PathIter, *PathEnd); ++PathIter)
        {
            if(!NearEqual(*PathIter, LastP, MaxDistSq))
            {
                LastP = *PathIter;
                Result.Points[Result.Count++] = LastP;
            }
        }

        if(IsClosedPath)
        {
            while((Result.Count > 1) &&
                  NearEqual(Result.Points[Result.Count - 1], FirstP, MaxDistSq))
            {
                Result.Points[--Result.Count] = {};
            }

            Result.Points = (v2_s64 *)realloc(Result.Points, sizeof(v2_s64)*Result.Count);
        }
    }

    return(Result);
}

inline paths_s64
StripNearEqual(paths_s64 *Source, f64 MaxDistSq, b32 IsClosedPath)
{
    paths_s64 Result = GetPathsS64(Source->PathCount);

    for(s32 I = 0;
        I < Source->PathCount;
        ++I)
    {
        Result.Paths[I] = StripNearEqual(Source->Paths + I, MaxDistSq, IsClosedPath);
    }

    return(Result);
}

inline void
StripDuplicates(path_s64 *Path, b32 IsClosedPath)
{
    for(s32 I = 0;
        I < (Path->Count - 1);
        ++I)
    {
        if(PointsAreEqual(Path->Points[I], Path->Points[I + 1]))
        {
            for(s32 J = I;
                J < (Path->Count - 1);
                ++J)
            {
                Path->Points[J] = Path->Points[J + 1];
            }

            --Path->Count;
            --I;
        }
    }

    if(IsClosedPath)
    {
        while((Path->Count > 1) &&
              PointsAreEqual(Path->Points[Path->Count - 1], Path->Points[0]))
        {
            Path->Points[--Path->Count] = {};
        }
    }

    Path->Points = (v2_s64 *)realloc(Path->Points, sizeof(v2_s64)*Path->Count);
}

inline void
StripDuplicates(paths_s64 *Source, b32 IsClosedPath)
{
    for(s32 I = 0;
        I < Source->PathCount;
        ++I)
    {
        StripDuplicates(Source->Paths + I, IsClosedPath);
    }
}

#define CLIPPER_CORE_H
#endif
