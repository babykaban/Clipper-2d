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

#ifdef CLIPPER_MAX_DECIMAL_PRECISION
  global_variable s32 CLIPPER_MAX_DEC_PRECISION = CLIPPER_MAX_DECIMAL_PRECISION;
#else
  global_variable s32 CLIPPER_MAX_DEC_PRECISION = 8; // see Discussions #564
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

#define RECORD_MEMORY_USEAGE 0

#if RECORD_MEMORY_USEAGE
global_variable u64 MemoryAllocated = 0;
global_variable u64 PathMemoryUsed = 0;

enum array_type
{
    ArrayType_OutRec,
    ArrayType_MinimaList,
    ArrayType_HorzSegList,
    ArrayType_HorzJoinList,
    ArrayType_IntersectNode,
    ArrayType_VertexLists,
    ArrayType_DeallocateActivesList,
    ArrayType_DeallocateOutPtsList,

    ArrayType_Count,
};

global_variable u32 ArrayMaxSizes[ArrayType_Count] = {};
#endif

#define BASIC_ALLOCATE_COUNT 16

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ZeroStruct(Instance) ZeroSize(sizeof(Instance), &(Instance))
#define ZeroArray(Count, Pointer) ZeroSize(Count*sizeof((Pointer)[0]), Pointer)
inline void
ZeroSize(umm Size, void *Ptr)
{
    u8 *Byte = (u8 *)Ptr;

#if CLIPPER_AVX
    __m256i Zero_32x = _mm256_set1_epi8(0);
    if(Size >= 32)
    {
        umm AVXSize = Size / 32;
        Size %= 32;

        for(umm I = 0; I < AVXSize; ++I)
        {
            _mm256_store_si256((__m256i *)Byte, Zero_32x);
            Byte += 32;
        }
    }
#elif CLIPPER_SSE
    __m128i Zero_16x = _mm_set1_epi8(0);
    if(Size >= 16)
    {
        umm SSESize = Size / 16;
        Size %= 16;

        for(umm I = 0; I < SSESize; ++I)
        {
            _mm_store_si128((__m128i *)Byte, Zero_16x);
            Byte += 16;
        }
    }
#endif
    
    while(Size--)
    {
        *Byte++ = 0;
    }
}

#define PushStruct(type, ...) (type *)PushSize_(sizeof(type), ## __VA_ARGS__)
#define PushArray(Count, type, ...) (type *)PushSize_((Count)*sizeof(type), ## __VA_ARGS__)
#define PushSize(Size, ...) PushSize_(Size, ## __VA_ARGS__)
#define PushCopy(Size, Source, ...) Copy(Size, Source, PushSize_(Arena, Size, ## __VA_ARGS__))

inline void *
PushSize_(umm SizeInit)
{
    void *Result = malloc(SizeInit);
    ZeroSize(SizeInit, Result);
    
#if RECORD_MEMORY_USEAGE
    MemoryAllocated += SizeInit;
#endif
    
    return(Result);
}

inline void *
Copy(umm Size, void *SourceInit, void *DestInit)
{
    u8 *Source = (u8 *)SourceInit;
    u8 *Dest = (u8 *)DestInit;
#if CLIPPER_AVX
    if(Size >= 32)
    {
        umm AVXSize = Size / 32;
        Size %= 32;

        for(umm I = 0; I < AVXSize; ++I)
        {
            __m256i Data = _mm256_load_si256((__m256i *)Source);
            _mm256_store_si256((__m256i *)Dest, Data);
            Source += 32;
            Dest += 32;
        }
    }
#elif CLIPPER_SSE
    if(Size >= 16)
    {
        umm SSESize = Size / 16;
        Size %= 16;

        for(umm I = 0; I < SSESize; ++I)
        {
            __m128i Data = _mm_load_si128((__m128i *)Source);
            _mm_store_si128((__m128i *)Dest, Data);
            Source += 16;
            Dest += 16;
        }
    }
#endif

    while(Size--) {*Dest++ = *Source++;}

    return(DestInit);
}

inline void *
Realloc(void *Base, umm Size, umm NewSize)
{
    void *Result = PushSize_(NewSize);
    if(Base)
    {
        Copy(Size, Base, Result);
        free(Base);
    }

#if RECORD_MEMORY_USEAGE
    MemoryAllocated -= Size;
#endif    
    return(Result);
}

#define ReallocArray(Base, OldCount, NewCount, type) (type *)Realloc(Base, (OldCount)*sizeof(type), (NewCount)*sizeof(type)) 

inline void *
Malloc(umm Size)
{
    void *Result = malloc(Size);
    ZeroSize(Size, Result);

#if RECORD_MEMORY_USEAGE
    MemoryAllocated += Size;
#endif
    
    return(Result);
}

#define MallocArray(Count, type) (type *)Malloc(sizeof(type)*(Count)); 
#define MallocStruct(type) (type *)Malloc(sizeof(type)) 

#if RECORD_MEMORY_USEAGE
inline void
Free(void *Ptr, umm Size)
{
    MemoryAllocated -= Size;

    free(Ptr);
}

#else

inline void
Free(void *Ptr, umm Size)
{
    free(Ptr);
}

#endif

inline b32
NeedIncrease(u32 Count)
{
    b32 Result = ((Count % BASIC_ALLOCATE_COUNT) == 0) && (Count != 0);
    return(Result);
}
// ===========================================================================================================================================================

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

#if RECORD_MEMORY_USEAGE
    PathMemoryUsed += BASIC_ALLOCATE_COUNT*sizeof(v2_f64);
#endif
}

inline void
IncreasePathS64(path_s64 *Path)
{
    Path->Points =
        ReallocArray(Path->Points, Path->Count,
                     Path->Count + BASIC_ALLOCATE_COUNT, v2_s64);
    Path->AllocateCount = Path->Count + BASIC_ALLOCATE_COUNT;

#if RECORD_MEMORY_USEAGE
    PathMemoryUsed += BASIC_ALLOCATE_COUNT*sizeof(v2_f64);
#endif
}

inline void
CheckPrecisionRange(s32 *Precision)
{
    if(!((*Precision >= -CLIPPER_MAX_DEC_PRECISION) &&
         (*Precision <= CLIPPER_MAX_DEC_PRECISION)))
    {
        *Precision = (*Precision > 0) ? CLIPPER_MAX_DEC_PRECISION : -CLIPPER_MAX_DEC_PRECISION;
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
    path_f64 Result = GetPathF64(Source->Count);
    Result.Count = Source->Count;
    if((Scale.x == 0) || (Scale.y == 0))
    {
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
    if((Scale.x == 0) || (Scale.y == 0))
    {
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
    path_f64 Result = GetPathF64(Source->Count);
    Result.Count = Source->Count;
    if((Scale.x == 0) || (Scale.y == 0))
    {
        if(Scale.x == 0)
            Scale.x = 1.0;
        if(Scale.y == 0)
            Scale.y = 1.0;
    }

#if 0
    // TODO: NOTE(babykaban): Not used right now, has to  be tested
    // avaliable only with AVX+ instruction sets
    v2_s64 *SourcePtr = Source->Points;
    v2_f64 *DestPtr = Result.Points;
    b32 OneLeft = Source->Count % 2;
    u32 LoopCount = OneLeft ? (Source->Count - 1) : Source->Count;
    for(u32 I = 0;
        I < LoopCount;
        I += 2)
    {
        __m256i TwoP = _mm256_load_si256((__m256i *)SourcePtr);
        __m256d Scale_2x = _mm256_set_m128d(Scale.W, Scale.W);
        __m256d TwoP_f64 = _mm256_mul_pd(_mm256_castsi256_pd(TwoP), Scale_2x);

        _mm256_store_pd((f64 *)DestPtr, TwoP_f64);

        SourcePtr += 2;
        DestPtr += 2;
    }

    if(OneLeft)
    {
        v2_s64 Point = *SourcePtr;
        __m128d Point_f64 = _mm_mul_pd(_mm_castsi128_pd(Point.W), Scale.W);
        _mm_store_pd((f64 *)DestPtr, Point_f64);
    }
#else    
    for(s32 I = 0;
        I < Source->Count;
        ++I)
    {
        Result.Points[I] = Scale*V2F64(Source->Points[I]);
    }
#endif
    return(Result);
}

inline path_s64
ScaleConvertPathToS64(path_f64 *Source, v2_f64 Scale)
{
    path_s64 Result = GetPathS64(Source->Count);
    Result.Count = Source->Count;
    if((Scale.x == 0) || (Scale.y == 0))
    {
        if(Scale.x == 0)
            Scale.x = 1.0;
        if(Scale.y == 0)
            Scale.y = 1.0;
    }

#if CLIPPER_AVX512
    // TODO: NOTE(babykaban): Only avaliable with AVX+ set of instructions
    // 272.57
    v2_f64 *SourcePtr = Source->Points;
    v2_s64 *DestPtr = Result.Points;
    b32 OneLeft = Source->Count % 2;
    u32 LoopCount = OneLeft ? (Source->Count - 1) : Source->Count;
    for(u32 I = 0;
        I < LoopCount;
        I += 2)
    {
        __m256d TwoP = _mm256_load_pd((f64 *)SourcePtr);
        __m256d Scale_2x = _mm256_set_m128d(Scale.W, Scale.W);
        TwoP = _mm256_mul_pd(TwoP, Scale_2x);

        __m128i FirstP_s64 = _mm_cvtpd_epi64(_mm_round_pd(_mm256_extractf128_pd(TwoP, 0),
                                                          _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
        __m128i SecondP_s64 = _mm_cvtpd_epi64(_mm_round_pd(_mm256_extractf128_pd(TwoP, 1),
                                                           _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));

        _mm_store_si128((__m128i *)DestPtr, FirstP_s64);
        _mm_store_si128((__m128i *)((s64 *)DestPtr + 2), SecondP_s64);

        SourcePtr += 2;
        DestPtr += 2;
    }

    if(OneLeft)
    {
        v2_f64 Point = *SourcePtr;
        Point.W = _mm_mul_pd(Point.W, Scale.W);
        __m128i Point_s64 = _mm_cvtpd_epi64(
            _mm_round_pd(Point.W, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
        _mm_store_si128((__m128i *)DestPtr, Point_s64);
    }
#else
    for(s32 I = 0;
        I < Source->Count;
        ++I)
    {
        Result.Points[I] = V2S64(Scale*Source->Points[I]);
    }
#endif    

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

#define CLIPPER_CORE_H
#endif
