#if !defined(MAIN_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "math.h"
#include <intrin.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef intptr_t intptr;
typedef uintptr_t uintptr;
    
typedef float real32;
typedef double real64;
    
typedef int8 s8;
typedef int8 s08;
typedef int16 s16;
typedef int32 s32;
typedef int64 s64;
typedef bool32 b32;

typedef uint8 u8;
typedef uint8 u08;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;

typedef real32 r32;
typedef real64 r64;
typedef real32 f32;
typedef real64 f64;

typedef uintptr_t umm;
typedef intptr_t  smm;

#if !defined(internal)
#define internal static
#endif
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f
#define Tau32 6.28318530717958647692f

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#define InvalidCodePath Assert(!"InvalidCodePath")
#define InvalidDefaultCase default: {InvalidCodePath;} break

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Minimum(A, B) ((A < B) ? (A) : (B))
#define Maximum(A, B) ((A > B) ? (A) : (B))

global_variable char *ClipTypes[] =
{
    "ClipType_NoClip",
    "ClipType_Intersection",
    "ClipType_Union",
    "ClipType_Difference",
    "ClipType_Xor",
};

global_variable char *FillRules[] =
{
    "FillRule_EvenOdd",
    "FillRule_NonZero",
    "FillRule_Positive",
    "FillRule_Negative",
};

struct v2_f32
{
    f32 x;
    f32 y;
};

struct polygon
{
    u32 Count;
    v2_f32 *Points;
};

struct polygon_set
{
    u32 PolyCount;
    polygon *Polygons;
};

#define PRINT_OUT_RESULT 0
#define PROFILER 1
#include "profiler.cpp"

#define MAIN_H
#endif
