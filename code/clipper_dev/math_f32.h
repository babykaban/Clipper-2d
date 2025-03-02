#if !defined(MATH_F32_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

inline f32
Square(f32 A)
{
    f32 Result = A*A;

    return(Result);
}

inline f32
Clamp(f32 Min, f32 Value, f32 Max)
{
    f32 Result = Value;

    if(Result < Min)
    {
        Result = Min;
    }
    else if(Result > Max)
    {
        Result = Max;
    }

    return(Result);
}

inline f32
Clamp01(f32 Value)
{
    f32 Result = Clamp(0.0f, Value, 1.0f);

    return(Result);
}

inline f32
Clamp01MapToRange(f32 Min, f32 t, f32 Max)
{
    f32 Result = 0.0;
    
    f32 Range = Max - Min;
    if(Range != 0.0)
    {
        Result = Clamp01((t - Min) / Range);
    }

    return(Result);
}

inline f32
SafeRatioN(f32 Numerator, f32 Divisor, f32 N)
{
    f32 Result = N;

    if(Divisor != 0.0f)
    {
        Result = Numerator / Divisor;
    }

    return(Result);
}

inline f32
SafeRatio0(f32 Numerator, f32 Divisor)
{
    f32 Result = SafeRatioN(Numerator, Divisor, 0.0f);

    return(Result);
}

inline f32
SafeRatio1(f32 Numerator, f32 Divisor)
{
    f32 Result = SafeRatioN(Numerator, Divisor, 1.0f);

    return(Result);
}

inline v2_f32
V2F32(f32 X, f32 Y)
{
    v2_f32 Result = {X, Y}; 

    return(Result);
}

// NOTE(babykaban): Double V2
inline v2_f32
V2F32(s64 X, s64 Y)
{
    v2_f32 Result = {(f32)X, (f32)Y}; 
    return(Result);
}

inline v2_f32
V2F32(v2_s64 A)
{
    v2_f32 Result = V2F32(A.x, A.y);
    return(Result);
}
 
inline v2_f32
operator*(f32 A, v2_f32 B)
{
    v2_f32 Result;

    Result.x = A*B.x;
    Result.y = A*B.y;

    return(Result);
}
 
inline v2_f32
operator*(v2_f32 A, v2_f32 B)
{
    v2_f32 Result;

    Result.x = A.x*B.x;
    Result.y = A.y*B.y;

    return(Result);
}

inline v2_f32
operator*(v2_f32 B, f32 A)
{
    v2_f32 Result = A*B;
    return(Result);
}

inline v2_f32 &
operator*=(v2_f32 &B, f32 A)
{
    B = A*B;

    return(B);
}

inline v2_f32
operator-(v2_f32 A)
{
    v2_f32 Result;

    Result.x = -A.x;
    Result.y = -A.y;

    return(Result);
}

inline v2_f32
operator+(v2_f32 A, v2_f32 B)
{
    v2_f32 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return(Result);
}

inline v2_f32 &
operator+=(v2_f32 &A, v2_f32 B)
{
    A = A + B;

    return(A);
}

inline v2_f32
operator-(v2_f32 A, v2_f32 B)
{
    v2_f32 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return(Result);
}

inline v2_f32 &
operator-=(v2_f32 &A, v2_f32 B)
{
    A = A - B;

    return(A);
}

inline f32
Inner(v2_f32 A, v2_f32 B)
{
    f32 Result = A.x*B.x + A.y*B.y;

    return(Result);
}

inline f32
Inner(v2_f32 A, v2_f32 B, v2_f32 C)
{
    v2_f32 AB = B - A;
    v2_f32 BC = C - A;
    f32 Result = Inner(AB, BC);

    return(Result);
}

inline f32
Cross(v2_f32 A, v2_f32 B)
{
    f32 Result = A.x*B.y - A.y*B.x;

    return(Result);
}

inline f32
Cross(v2_f32 A, v2_f32 B, v2_f32 C)
{
    v2_f32 AB = B - A;
    v2_f32 BC = C - A;
    f32 Result = Cross(AB, BC);

    return(Result);
}

inline f32
LengthSq(v2_f32 A)
{
    f32 Result = Inner(A, A);

    return(Result);
}

inline f32
Length(v2_f32 A)
{
    f32 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline b32
PointsAreEqual(v2_f32 A, v2_f32 B)
{
    b32 Result = ((A.x == B.x) && (A.y == B.y));
    return(Result);
}

inline v2_f32
MidPointF32(v2_f32 p1, v2_f32 p2)
{
    v2_f32 Result;
    Result = 0.5*(p1 + p2);
    return(Result);
}

inline b32
NearEqual(v2_f32 p1, v2_f32 p2, f32 MaxDistSq)
{
    v2_f32 p21 = p1 - p2;

    b32 Result = (Square(p21.x) + Square(p21.y) < MaxDistSq);
    return(Result);
}


inline f32
PerpDistFromLineSq(v2_f32 p, v2_f32 a, v2_f32 b)
{
    f32 Result = 0.0;

    v2_f32 ap = p - a;
    v2_f32 ab = b - a;
    if(!((ab.x == 0) && (ab.y == 0)))
    {
        Result = Square(Cross(ap, ab)) / Inner(ab, ab);
    }

    return(Result);
}

inline b32
IsCollinear(v2_f32 p1, v2_f32 SharedP, v2_f32 p2) // #777
{
    s64 a = RoundF32ToInt64(SharedP.x - p1.x);
    s64 b = RoundF32ToInt64(p2.y - SharedP.y);
    s64 c = RoundF32ToInt64(SharedP.y - p1.y);
    s64 d = RoundF32ToInt64(p2.x - SharedP.x);
    // When checking for collinearity with very large coordinate values
    // then ProductsAreEqual is more accurate than using CrossProduct.
    b32 Result = ProductsAreEqual(a, b, c, d);
    return(Result);
}

#define CLIPPER2_HI_PRECISION 0
#if CLIPPER2_HI_PRECISION
// caution: this will compromise performance
// https://github.com/AngusJohnson/Clipper2/issues/317#issuecomment-1314023253
// See also CPP/BenchMark/GetIntersectPtBenchmark.cpp
#define CC_MIN(x,y) ((x)>(y)?(y):(x))
#define CC_MAX(x,y) ((x)<(y)?(y):(x))

inline b32
GetSegmentIntersectPt(v2_f32 a, v2_f32 b, v2_f32 c, v2_f32 d, v2_f32 *p)
{
    b32 Result = false;

    f32 ln1dy = (b.y - a.y);
    f32 ln1dx = (a.x - b.x);
    f32 ln2dy = (d.y - c.y);
    f32 ln2dx = (c.x - d.x);
    f32 det = (ln2dy * ln1dx) - (ln1dy * ln2dx);

    if(det != 0.0)
    {
        f32 bb0minx = CC_MIN(a.x, b.x);
        f32 bb0miny = CC_MIN(a.y, b.y);
        f32 bb0maxx = CC_MAX(a.x, b.x);
        f32 bb0maxy = CC_MAX(a.y, b.y);
        f32 bb1minx = CC_MIN(c.x, d.x);
        f32 bb1miny = CC_MIN(c.y, d.y);
        f32 bb1maxx = CC_MAX(c.x, d.x);
        f32 bb1maxy = CC_MAX(c.y, d.y);

        f32 originx = (CC_MIN(bb0maxx, bb1maxx) + CC_MAX(bb0minx, bb1minx)) / 2.0;
        f32 originy = (CC_MIN(bb0maxy, bb1maxy) + CC_MAX(bb0miny, bb1miny)) / 2.0;
        f32 ln0c = (ln1dy * (a.x - originx)) + (ln1dx * (a.y - originy));
        f32 ln1c = (ln2dy * (c.x - originx)) + (ln2dx * (c.y - originy));
        f32 hitx = ((ln1dx * ln1c) - (ln2dx * ln0c)) / det;
        f32 hity = ((ln2dy * ln0c) - (ln1dy * ln1c)) / det;

        p->x = originx + hitx;
        p->y = originy + hity;
        Result = true;
    }

    return(Result);
}

#else

inline b32
GetSegmentIntersectPt(v2_f32 a, v2_f32 b, v2_f32 c, v2_f32 d, v2_f32 *p)
{
    // https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection

    b32 Result = false;
    
    v2_f32 ab = b - a;
    v2_f32 cd = d - c;

    f32 det = Cross(cd, ab);
    if(det != 0.0)
    {
        f32 t = ((a.x - c.x) * cd.y - (a.y - c.y) * cd.x) / det;
        if(t <= 0.0) *p = a;
        else if(t >= 1.0) *p = b;
        else
        {
            *p = a + t*ab;
        }
    }

    return true;
}
#endif

inline v2_f32
TranslatePoint(v2_f32 p, v2_f32 Offset)
{
    // TODO(babykaban): Eliminate
    v2_f32 Result = p + Offset;
    return(Result);
}

inline v2_f32
ReflectPoint(v2_f32 p, v2_f32 pivot)
{
    v2_f32 Result = pivot + (pivot - p);
    return(Result);
}

inline s32
GetSign(f32 Val) 
{ 
    s32 Result = 0;
    if(Val)
    {
        Result =  (Val > 0) ? 1 : -1;
    }

    return(Result);
}

inline v2_f32
GetClosestPointOnSegment(v2_f32 offPt, v2_f32 seg1, v2_f32 seg2)
{
    v2_f32 Result = {};
    
    if(PointsAreEqual(seg1, seg2))
    {
        Result = seg1;
    }
    else
    {
        v2_f32 d = seg2 - seg1;

        f32 q = Inner(offPt - seg1, d) / Inner(d, d);
        Clamp01(q);

        Result = seg1 + q*d;
    }

    return(Result);
}

#define MATH_F32_H
#endif
