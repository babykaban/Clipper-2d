#if !defined(CLIPPER_MATH_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

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

#define CLIPPER_MATH_H
#endif
