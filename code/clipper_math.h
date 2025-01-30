#if !defined(CLIPPER_MATH_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

// https://github.com/AngusJohnson/Clipper2/discussions/334
// #discussioncomment-4248602
#if defined(_MSC_VER) && ( defined(_M_AMD64) || defined(_M_X64) )
#include <xmmintrin.h>
#include <emmintrin.h>
#define fmin(a,b) _mm_cvtsd_f64(_mm_min_sd(_mm_set_sd(a),_mm_set_sd(b)))
#define fmax(a,b) _mm_cvtsd_f64(_mm_max_sd(_mm_set_sd(a),_mm_set_sd(b)))
#define nearbyint(a) _mm_cvtsd_si64(_mm_set_sd(a)) /* Note: expression type is (int64_t) */
#endif

union v2_s64
{
    struct
    {
        s64 x, y;
    };

    __m128i W;
    s64 E[2];
};

struct path_s64
{
    s32 Count;
    u32 AllocateCount;

    v2_s64 *Points;
};

struct paths_s64
{
    s32 PathCount;
    u32 AllocateCount;

    path_s64 *Paths;
};

union v2_f64
{
    struct
    {
        f64 x, y;
    };

    __m128d W;
    f64 E[2];
};

struct path_f64
{
    s32 Count;
    u32 AllocateCount;

    v2_f64 *Points;
};

struct paths_f64
{
    s32 PathCount;
    u32 AllocateCount;

    path_f64 *Paths;
};

struct rectangle2
{
    v2_f64 Min;
    v2_f64 Max;
};

struct rectangle2i
{
    v2_s64 Min;
    v2_s64 Max;
};

inline f64
SquareRoot(f64 Float64)
{
    f64 Result = sqrt((float)Float64);
    return(Result);
}

inline f32
SquareRoot(f32 Float32)
{
    f32 Result = sqrtf(Float32);
    return(Result);
}

inline s64
RoundF64ToInt64(f64 Value)
{
    s64 Result = (s64)round(Value);
    return(Result);
}

inline s64
RoundF32ToInt64(f32 Value)
{
    s64 Result = (s64)roundf(Value);
    return(Result);
}

inline s32
TriSign(s64 x) // returns 0, 1 or -1
{
    s32 Result = ((x > 0) - (x < 0));
    return(Result); 
}

struct mul_u64_res
{
    u64 Value;
    u64 Carry;
};

inline b32
AreMulsEqual(mul_u64_res A, mul_u64_res B)
{
    b32 Result = ((A.Value == B.Value) && (A.Carry == B.Carry));
    return(Result);
}

inline u64
Low(u64 Value)
{
    u64 Result = (Value & 0xFFFFFFFF); 
    return(Result);
}

inline u64
High(u64 Value)
{
    u64 Result = (Value >> 32); 
    return(Result);
}

inline mul_u64_res
MultiplyU64(u64 a, u64 b) // #834, #835
{
    mul_u64_res Result = {};
    
    u64 x1 = Low(a) * Low(b);
    u64 x2 = (High(a) * Low(b)) + High(x1);
    u64 x3 = (Low(a) * High(b)) + Low(x2);
    Result.Value = (Low(x3) << 32) | Low(x1);
    Result.Carry = (High(a) * High(b)) + High(x2) + High(x3);

    return(Result);
}

// returns true if (and only if) a * b == c * d
inline b32
ProductsAreEqual(s64 a, s64 b, s64 c, s64 d)
{
    // nb: unsigned values needed for calculating overflow carry
    u64 abs_a = (u64)abs(a);
    u64 abs_b = (u64)abs(b);
    u64 abs_c = (u64)abs(c);
    u64 abs_d = (u64)abs(d);
    mul_u64_res abs_ab = MultiplyU64(abs_a, abs_b);
    mul_u64_res abs_cd = MultiplyU64(abs_c, abs_d);

    // nb: it's important to differentiate 0 values here from other values
    s32 sign_ab = TriSign(a) * TriSign(b);
    s32 sign_cd = TriSign(c) * TriSign(d);

    s32 Result = (AreMulsEqual(abs_ab, abs_cd) && (sign_ab == sign_cd));
    
    return(Result);
}


struct cp_v
{
    v2_s64 A;
    v2_s64 B;
};

struct cp_entry
{
    cp_v Key;
    f64 Result;

    cp_entry *Next;
};

struct cp_hash_table
{
    cp_entry **Entries;
    u32 Size;
};

global_variable cp_hash_table GlobalCrossTable = {};

inline u64
Hash(cp_v V)
{
    u64 hash = V.A.x + (V.B.x << 6) + (V.A.y << 16) - V.B.y;
    
    return hash;
}

inline cp_entry *
CreateEntry(cp_v Key, f64 Value)
{
    cp_entry *Entry = (cp_entry *)malloc(sizeof(cp_entry));
    Entry->Key = Key;
    Entry->Result = Value;
    Entry->Next = 0;

    return(Entry);
}

#define TABLE_SIZE 4096

inline void
Insert(cp_hash_table *Table, cp_v key, f64 Value)
{
    u32 Index = Hash(key) % TABLE_SIZE;
    cp_entry *Entry = CreateEntry(key, Value); 
    Entry->Next = Table->Entries[Index];
    Table->Entries[Index] = Entry;
    Table->Size++;
}

inline b32
PointsAreEqual_(v2_s64 A, v2_s64 B)
{
    b32 Result = ((A.x == B.x) && (A.y == B.y));
    return(Result);
}

inline cp_entry *
Get(cp_hash_table *Table, cp_v key)
{
    u32 Index = Hash(key) % TABLE_SIZE;
    cp_entry *Entry = Table->Entries[Index]; 
    while(Entry)
    {
        if(PointsAreEqual_(Entry->Key.A, key.A) &&
           PointsAreEqual_(Entry->Key.B, key.B))
        {
            break;
        }

        Entry = Entry->Next;
    }

    return(Entry);
}

internal void
ClearCPTable(cp_hash_table *Table)
{
    for(int i = 0; i < TABLE_SIZE; i++)
    {
        cp_entry *node = Table->Entries[i];
        while(node)
        {
            cp_entry *tmp = node;
            node = node->Next;
            free(tmp);
        }
    }
}

#define CLIPPER_MATH_H
#endif
