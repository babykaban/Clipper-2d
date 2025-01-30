#if !defined(CLIPPER_MATH_F64_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

inline b32
IsValid(rectangle2 Rect)
{
    b32 Result = (Rect.Min.x != DBL_MAX);
    return(Result);
}

inline f64
Square(f64 A)
{
    f64 Result = A*A;

    return(Result);
}

inline f64
Clamp(f64 Min, f64 Value, f64 Max)
{
    f64 Result = Value;

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

inline f64
Clamp01(f64 Value)
{
    f64 Result = Clamp(0.0, Value, 1.0);

    return(Result);
}

inline f64
Clamp01MapToRange(f64 Min, f64 t, f64 Max)
{
    f64 Result = 0.0;
    
    f64 Range = Max - Min;
    if(Range != 0.0)
    {
        Result = Clamp01((t - Min) / Range);
    }

    return(Result);
}

inline f64
SafeRatioN(f64 Numerator, f64 Divisor, f64 N)
{
    f64 Result = N;

    if(Divisor != 0.0)
    {
        Result = Numerator / Divisor;
    }

    return(Result);
}

inline f64
SafeRatio0(f64 Numerator, f64 Divisor)
{
    f64 Result = SafeRatioN(Numerator, Divisor, 0.0);

    return(Result);
}

inline f64
SafeRatio1(f64 Numerator, f64 Divisor)
{
    f64 Result = SafeRatioN(Numerator, Divisor, 1.0);

    return(Result);
}

inline v2_f64
V2F64(f64 X, f64 Y)
{
    v2_f64 Result = {X, Y}; 

    return(Result);
}

// NOTE(babykaban): Double V2
inline v2_f64
V2F64(s64 X, s64 Y)
{
    v2_f64 Result = {(f64)X, (f64)Y}; 
    return(Result);
}

inline v2_f64
V2F64(v2_s64 A)
{
    v2_f64 Result = V2F64(A.x, A.y);
    return(Result);
}

inline v2_f64
V2F64(v2_f32 A)
{
    v2_f64 Result = {A.x, A.y};
    return(Result);
}
 
inline v2_f64
operator*(f64 A, v2_f64 B)
{
    v2_f64 Result;

    Result.W = _mm_mul_pd(B.W, _mm_set1_pd(A));
//    Result.x = A*B.x;
//    Result.y = A*B.y;

    return(Result);
}
 
inline v2_f64
operator*(v2_f64 A, v2_f64 B)
{
    v2_f64 Result;

    Result.W = _mm_mul_pd(B.W, A.W);
//    Result.x = A*B.x;
//    Result.y = A*B.y;

    return(Result);
}

inline v2_f64
operator*(v2_f64 B, f64 A)
{
    v2_f64 Result = A*B;
    return(Result);
}

inline v2_f64 &
operator*=(v2_f64 &B, f64 A)
{
    B = A*B;

    return(B);
}

inline v2_f64
operator-(v2_f64 A)
{
    v2_f64 Result;

    Result.W = _mm_xor_pd(A.W, _mm_set1_pd(-0.0));
//    Result.x = -A.x;
//    Result.y = -A.y;

    return(Result);
}

inline v2_f64
operator+(v2_f64 A, v2_f64 B)
{
    v2_f64 Result;

    Result.W = _mm_add_pd(A.W, B.W);
//    Result.x = A.x + B.x;
//    Result.y = A.y + B.y;

    return(Result);
}

inline v2_f64 &
operator+=(v2_f64 &A, v2_f64 B)
{
    A = A + B;

    return(A);
}

inline v2_f64
operator-(v2_f64 A, v2_f64 B)
{
    v2_f64 Result;

    Result.W = _mm_sub_pd(A.W, B.W);
//    Result.x = A.x - B.x;
//    Result.y = A.y - B.y;

    return(Result);
}

inline v2_f64 &
operator-=(v2_f64 &A, v2_f64 B)
{
    A = A - B;

    return(A);
}

inline f64
Inner(v2_f64 A, v2_f64 B)
{
    v2_f64 Mul = A*B;
    f64 Result = _mm_cvtsd_f64(_mm_hadd_pd(Mul.W, Mul.W));
//    f64 Result = A.x*B.x + A.y*B.y;

    return(Result);
}

inline f64
Inner(v2_f64 A, v2_f64 B, v2_f64 C)
{
    v2_f64 AB = B - A;
    v2_f64 BC = C - A;
    f64 Result = Inner(AB, BC);

    return(Result);
}

inline f64
Cross(v2_f64 A, v2_f64 B)
{
    TimeBlock("Cross FF");
    
    f64 Result = 0.0;
#if 0
    cp_v E = {};
    E.A = {nearbyint(A.x), nearbyint(A.y)};
    E.B = {nearbyint(B.x), nearbyint(B.y)};

    cp_entry *Ent = Get(&GlobalCrossTable, E);
    if(Ent)
    {
        Result = Ent->Result;
    }
    else
    {
    
        Result = A.x*B.y - A.y*B.x;
        Insert(&GlobalCrossTable, E, Result);
    }
#else
    Result = A.x*B.y - A.y*B.x;
#endif
    
#if 0
    // Shuffle A and B to get {A.y, A.x} and {B.y, B.x}
    __m128d AyAx = _mm_shuffle_pd(A.W, A.W, 1); // Swaps A.x and A.y
    __m128d ByBx = _mm_shuffle_pd(B.W, B.W, 1); // Swaps B.x and B.y

    // Compute A.x * B.y and A.y * B.x
    __m128d prod1 = _mm_mul_pd(A.W, ByBx); // {A.x * B.y, A.y * B.x}
    __m128d prod2 = _mm_mul_pd(AyAx, B.W); // {A.y * B.x, A.x * B.y}

    // Subtract the results to get A.x * B.y - A.y * B.x in the lower lane
    __m128d result = _mm_sub_sd(prod1, prod2);

    f64 Result = _mm_cvtsd_f64(result);;
#endif

    return(Result);
}

inline f64
Cross(v2_f64 A, v2_f64 B, v2_f64 C)
{
    v2_f64 AB = B - A;
    v2_f64 BC = C - A;
    f64 Result = Cross(AB, BC);

    return(Result);
}

inline f64
LengthSq(v2_f64 A)
{
    f64 Result = Inner(A, A);

    return(Result);
}

inline f64
Length(v2_f64 A)
{
    f64 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline b32
PointsAreEqual(v2_f64 A, v2_f64 B)
{
    b32 Result = ((A.x == B.x) && (A.y == B.y));
    return(Result);
}

inline v2_f64
MidPointF64(v2_f64 p1, v2_f64 p2)
{
    v2_f64 Result;
    Result = 0.5*(p1 + p2);
    return(Result);
}

inline rectangle2
InvertedInfinityRectangle2(void)
{
    rectangle2 Result;

    Result.Min.x = Result.Min.y = DBL_MAX;
    Result.Max.x = Result.Max.y = -DBL_MAX;

    return(Result);
}

inline v2_f64
GetMinCorner(rectangle2 Rect)
{
    v2_f64 Result = Rect.Min;
    return(Result);
}

inline v2_f64
GetMaxCorner(rectangle2 Rect)
{
    v2_f64 Result = Rect.Max;
    return(Result);
}

inline v2_f64
GetDim(rectangle2 Rect)
{
    v2_f64 Result = Rect.Max - Rect.Min;
    return(Result);
}

inline v2_f64
GetCenter(rectangle2 Rect)
{
    v2_f64 Result = 0.5f*(Rect.Min + Rect.Max);
    return(Result);
}

inline f64
GetArea(rectangle2 A)
{
    v2_f64 Dim = GetDim(A);
    f64 Result = Dim.x*Dim.y;

    return(Result);
}

inline rectangle2
RectMinMax(v2_f64 Min, v2_f64 Max)
{
    rectangle2 Result;

    Result.Min = Min;
    Result.Max = Max;

    return(Result);
}

inline rectangle2
RectMinDim(v2_f64 Min, v2_f64 Dim)
{
    rectangle2 Result;

    Result.Min = Min;
    Result.Max = Min + Dim;

    return(Result);
}

inline rectangle2
RectCenterHalfDim(v2_f64 Center, v2_f64 HalfDim)
{
    rectangle2 Result;

    Result.Min = Center - HalfDim;
    Result.Max = Center + HalfDim;

    return(Result);
}

inline rectangle2
AddRadiusTo(rectangle2 A, v2_f64 Radius)
{
    rectangle2 Result;
    Result.Min = A.Min - Radius;
    Result.Max = A.Max + Radius;

    return(Result);
}

inline rectangle2
ScaleRect(rectangle2 Rect, f64 Scale)
{
    rectangle2 Result = {};
    Result.Min = Scale*Rect.Min;
    Result.Max = Scale*Rect.Max;

    return(Result);
}

inline rectangle2
Offset(rectangle2 A, v2_f64 Offset)
{
    rectangle2 Result;

    Result.Min = A.Min + Offset;
    Result.Max = A.Max + Offset;

    return(Result);
}

inline rectangle2
RectCenterDim(v2_f64 Center, v2_f64 Dim)
{
    rectangle2 Result = RectCenterHalfDim(Center, 0.5f*Dim);

    return(Result);
}

inline b32
IsInRectangle(rectangle2 Rectangle, v2_f64 Test)
{
    bool32 Result = ((Test.x > Rectangle.Min.x) &&
                     (Test.y > Rectangle.Min.y) &&
                     (Test.x < Rectangle.Max.x) &&
                     (Test.y < Rectangle.Max.y));

    return(Result);
}

inline b32
IsInRectangle(rectangle2 Rect, rectangle2 Test)
{
    bool32 Result = ((Test.Min.x >= Rect.Min.x) &&
                     (Test.Min.y >= Rect.Min.y) &&
                     (Test.Max.x <= Rect.Max.x) &&
                     (Test.Max.y <= Rect.Max.y));

    return(Result);
}

inline b32
Intersects(rectangle2 A, rectangle2 B)
{
    b32 Result = ((Maximum(A.Min.x, B.Min.x) <= Minimum(A.Max.x, B.Max.x)) &&
                  (Maximum(A.Max.y, B.Max.y) <= Minimum(A.Min.y, B.Min.y)));

    return(Result);
}

inline b32
IsRectEmpty(rectangle2 Rect)
{
    b32 Result = ((Rect.Min.y <= Rect.Max.y) ||
                  (Rect.Max.x <= Rect.Min.x));

    return(Result);
}

inline v2_f64
GetBarycentric(rectangle2 A, v2_f64 P)
{
    v2_f64 Result;

    Result.x = SafeRatio0(P.x - A.Min.x, A.Max.x - A.Min.x);
    Result.y = SafeRatio0(P.y - A.Min.y, A.Max.y - A.Min.y);

    return(Result);
}

inline b32
RectsAreEqual(rectangle2 A, rectangle2 B)
{
    b32 Result = (PointsAreEqual(A.Min, B.Min) &&
                  PointsAreEqual(A.Max, B.Max));
    return(Result);
}

inline rectangle2
Union(rectangle2 A, rectangle2 B)
{
    rectangle2 Result;
    
    Result.Min.x = Minimum(A.Min.x, B.Min.x);
    Result.Min.y = Minimum(A.Min.y, B.Min.y);
    Result.Max.x = Maximum(A.Max.x, B.Max.x);
    Result.Max.y = Maximum(A.Max.y, B.Max.y);

    return(Result);
}

inline rectangle2
GetBounds(path_f64 *Path)
{
    rectangle2 Result = InvertedInfinityRectangle2();

    for(s32 I = 0;
        I < Path->Count;
        ++I)
    {
        v2_f64 p = Path->Points[I];
        if (p.x < Result.Min.x) Result.Min.x = p.x;
        if (p.x > Result.Max.x) Result.Max.x = p.x;
        if (p.y < Result.Min.y) Result.Min.y = p.y;
        if (p.y > Result.Max.y) Result.Max.y = p.y;
    }

    return(Result);
}

inline rectangle2
GetBounds(paths_f64 *Paths)
{
    rectangle2 Result = InvertedInfinityRectangle2();

    Result = GetBounds(Paths->Paths + 0);
    for(s32 I = 1;
        I < Paths->PathCount;
        ++I)
    {
        path_f64 *Path = Paths->Paths + I;
        rectangle2 Bounds = GetBounds(Path);
        Result = Union(Result, Bounds);
    }

    return(Result);
}

inline b32
NearEqual(v2_f64 p1, v2_f64 p2, f64 MaxDistSq)
{
    v2_f64 p21 = p1 - p2;

    b32 Result = (Square(p21.x) + Square(p21.y) < MaxDistSq);
    return(Result);
}


inline f64
PerpDistFromLineSq(v2_f64 p, v2_f64 a, v2_f64 b)
{
    f64 Result = 0.0;

    v2_f64 ap = p - a;
    v2_f64 ab = b - a;
    if(!((ab.x == 0) && (ab.y == 0)))
    {
        Result = Square(Cross(ap, ab)) / Inner(ab, ab);
    }

    return(Result);
}

inline f64
Area(path_f64 *Path)
{
    TimeFunction;
    f64 Result = 0.0;

    s32 Count = Path->Count;
    if(Count >= 3)
    {
        v2_f64 *Iter1 = Path->Points + (Count - 2);
        v2_f64 *Iter2 = Iter1;
        v2_f64 *Stop = Iter2;

        if(!(Count & 1))
        {
            ++Stop;
        }

        for(Iter1 = Path->Points;
            !PointsAreEqual(*Iter1, *Stop);)
        {
            Result += (Iter2->y + Iter1->y) * (Iter2->x - Iter1->x);
            Iter2 = Iter1 + 1;
            Result += (Iter1->y + Iter2->y) * (Iter1->x - Iter2->x);
            Iter1 += 2;
        }

        if(Count & 1)
        {
            Result += (Iter2->y + Iter1->y) * (Iter2->x - Iter1->x);
        }

        Result *= 0.5;
    }

    return(Result);
}

inline f64
Area(paths_f64 *Paths)
{
    f64 Result = 0.0;
    for(s32 I = 0;
        I < Paths->PathCount;
        ++I)
    {
        Result += Area(Paths->Paths + I);
    }

    return(Result);
}

inline b32
IsPositive(path_f64 *Poly)
{
    // A curve has positive orientation [and area] if a region 'R'
    // is on the left when traveling around the outside of 'R'.
    //https://mathworld.wolfram.com/CurveOrientation.html
    //nb: This statement is premised on using Cartesian coordinates
    b32 Result = (Area(Poly) >= 0);
    return(Result);
}

inline b32
IsCollinear(v2_f64 p1, v2_f64 SharedP, v2_f64 p2) // #777
{
    s64 a = RoundF64ToInt64(SharedP.x - p1.x);
    s64 b = RoundF64ToInt64(p2.y - SharedP.y);
    s64 c = RoundF64ToInt64(SharedP.y - p1.y);
    s64 d = RoundF64ToInt64(p2.x - SharedP.x);
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
GetSegmentIntersectPt(v2_f64 a, v2_f64 b, v2_f64 c, v2_f64 d, v2_f64 *p)
{
    b32 Result = false;

    f64 ln1dy = (b.y - a.y);
    f64 ln1dx = (a.x - b.x);
    f64 ln2dy = (d.y - c.y);
    f64 ln2dx = (c.x - d.x);
    f64 det = (ln2dy * ln1dx) - (ln1dy * ln2dx);

    if(det != 0.0)
    {
        f64 bb0minx = CC_MIN(a.x, b.x);
        f64 bb0miny = CC_MIN(a.y, b.y);
        f64 bb0maxx = CC_MAX(a.x, b.x);
        f64 bb0maxy = CC_MAX(a.y, b.y);
        f64 bb1minx = CC_MIN(c.x, d.x);
        f64 bb1miny = CC_MIN(c.y, d.y);
        f64 bb1maxx = CC_MAX(c.x, d.x);
        f64 bb1maxy = CC_MAX(c.y, d.y);

        f64 originx = (CC_MIN(bb0maxx, bb1maxx) + CC_MAX(bb0minx, bb1minx)) / 2.0;
        f64 originy = (CC_MIN(bb0maxy, bb1maxy) + CC_MAX(bb0miny, bb1miny)) / 2.0;
        f64 ln0c = (ln1dy * (a.x - originx)) + (ln1dx * (a.y - originy));
        f64 ln1c = (ln2dy * (c.x - originx)) + (ln2dx * (c.y - originy));
        f64 hitx = ((ln1dx * ln1c) - (ln2dx * ln0c)) / det;
        f64 hity = ((ln2dy * ln0c) - (ln1dy * ln1c)) / det;

        p->x = originx + hitx;
        p->y = originy + hity;
        Result = true;
    }

    return(Result);
}

#else

inline b32
GetSegmentIntersectPt(v2_f64 a, v2_f64 b, v2_f64 c, v2_f64 d, v2_f64 *p)
{
    TimeFunction;
    // https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection

    b32 Result = false;
    
    v2_f64 ab = b - a;
    v2_f64 cd = d - c;

    f64 det = Cross(cd, ab);
    if(det != 0.0)
    {
        f64 t = ((a.x - c.x) * cd.y - (a.y - c.y) * cd.x) / det;
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

inline v2_f64
TranslatePoint(v2_f64 p, v2_f64 Offset)
{
    // TODO(babykaban): Eliminate
    v2_f64 Result = p + Offset;
    return(Result);
}

inline v2_f64
ReflectPoint(v2_f64 p, v2_f64 pivot)
{
    v2_f64 Result = pivot + (pivot - p);
    return(Result);
}

inline s32
GetSign(f64 Val) 
{ 
    s32 Result = 0;
    if(Val)
    {
        Result =  (Val > 0) ? 1 : -1;
    }

    return(Result);
}

inline v2_f64
GetClosestPointOnSegment(v2_f64 offPt, v2_f64 seg1, v2_f64 seg2)
{
    TimeFunction;
    v2_f64 Result = {};
    
    if(PointsAreEqual(seg1, seg2))
    {
        Result = seg1;
    }
    else
    {
        v2_f64 d = seg2 - seg1;

        f64 q = Inner(offPt - seg1, d) / Inner(d, d);
        Clamp01(q);

        Result = seg1 + q*d;
    }

    return(Result);
}

enum point_in_poly_res
{
    PointInPolyRes_On,
    PointInPolyRes_Inside,
    PointInPolyRes_Outside,
};

inline point_in_poly_res
PointInPolygon(v2_f64 p, path_f64 *Poly)
{
    point_in_poly_res Result = PointInPolyRes_Outside;

    s32 val = 0;
    if(Poly->Count >= 3)
    {
        v2_f64 *Start = Poly->Points + 0;
        v2_f64 *End = Poly->Points + (Poly->Count - 1);
        v2_f64 *First = Start;
        v2_f64 *Cur;
        v2_f64 *Prev;

        while (PointsAreEqual(*First, *End) && (First->y == p.y))
            ++First;

        if(!PointsAreEqual(*First, *End))
        {
            b32 IsAbove = (First->y < p.y);
            b32 StartingAbove = IsAbove;

            Cur = First + 1;
            while(true)
            {
                if(PointsAreEqual(*Cur, *End))
                {
                    if(PointsAreEqual(*End, *First) ||
                       PointsAreEqual(*First, *Start))
                    {
                        break;
                    }

                    End = First;
                    Cur = Start;
                }

                if(IsAbove)
                {
                    while(!PointsAreEqual(*Cur, *End) && (Cur->y < p.y))
                        ++Cur;
                }
                else
                {
                    while(!PointsAreEqual(*Cur, *End) && (Cur->y > p.y))
                        ++Cur;
                }

                if(!PointsAreEqual(*Cur, *End))
                {
                    if(PointsAreEqual(*Cur, *Start))
                        Prev = Poly->Points + (Poly->Count - 2); //nb: NOT cend (since might equal first)
                    else
                        Prev = Cur - 1;

                    if(Cur->y == p.y)
                    {
                        if((Cur->x == p.x) ||
                           ((Cur->y == Prev->y) &&
                            ((p.x < Prev->x) != (p.x < Cur->x))))
                        {
                            Result = PointInPolyRes_On;
                            break;
                        }
                        else
                        {
                            ++Cur;
                            if(PointsAreEqual(*Cur, *First))
                                break;
                        }
                    }
                    else
                    {
                        if((p.x < Cur->x) && (p.x < Prev->x))
                        {
                            // we're only interested in edges crossing on the left
                        }
                        else if((p.x > Prev->x) && (p.x > Cur->x))
                            val = 1 - val; // toggle val
                        else
                        {
                            f64 d = Cross(*Prev, *Cur, p);
                            if(d == 0)
                            {
                                Result = PointInPolyRes_On;
                                break;
                            }
                            else if ((d < 0) == IsAbove)
                                val = 1 - val;
                        }

                        IsAbove = !IsAbove;
                        ++Cur;
                    }
                }
            }

            if((IsAbove != StartingAbove) && (Result != PointInPolyRes_On))
            {
                End = Poly->Points + (Poly->Count - 1);
                if(PointsAreEqual(*Cur, *End))
                    Cur = Start;
                if(PointsAreEqual(*Cur, *Start))
                    Prev = End - 1;
                else
                    Prev = Cur - 1;

                f64 d = Cross(*Prev, *Cur, p);
                if(d == 0)
                    Result = PointInPolyRes_On;
                else if ((d < 0) == IsAbove)
                    val = 1 - val;
            }
        }
    }

    if(Result != PointInPolyRes_On)
        Result = (val == 0) ? PointInPolyRes_Outside : PointInPolyRes_Inside;

    return(Result);
}

#define CLIPPER_MATH_F64_H
#endif
