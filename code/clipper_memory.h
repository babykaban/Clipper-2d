#if !defined(CLIPPER_MEMORY_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

#define RECORD_MEMORY_USEAGE 1

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

    __m256i Zero_32x = _mm256_set1_epi8(0);
    __m128i Zero_16x = _mm_set1_epi8(0);
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


#define CLIPPER_MEMORY_H
#endif
