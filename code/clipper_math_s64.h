#if !defined(CLIPPER_MATH_S64_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

inline b32
IsValid(rectangle2i Rect)
{
    b32 Result = (Rect.Min.x != DBL_MAX);
    return(Result);
}

inline f64
Square(s64 A)
{
    f64 Result = (f64)A*(f64)A;

    return(Result);
}

inline v2_s64
V2S64(s64 X, s64 Y)
{
    v2_s64 Result = {X, Y}; 
    return(Result);
}

inline v2_s64
V2S64(f64 X, f64 Y)
{
    v2_s64 Result = {};
#if 1
    Result.W = _mm_cvtpd_epi64(_mm_round_pd(_mm_set_pd(X, Y),
                                            _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
#else
    Result.x = (s64)round(X);
    Result.y = (s64)round(Y);
#endif
    
    return(Result);
}

inline v2_s64
CastV2S64(v2_f64 A)
{
    v2_s64 Result = {(s64)A.x, (s64)A.y}; 

    return(Result);
}

inline v2_s64
V2S64(v2_f64 A)
{
    v2_s64 Result = V2S64(A.x, A.y);
    return(Result);
}

inline v2_s64
operator*(f64 A, v2_s64 B)
{
    v2_s64 Result;
    Result = V2S64(V2F64(B)*A);
    return(Result);
}

inline v2_s64
operator*(v2_s64 B, f64 A)
{
    v2_s64 Result = A*B;
    return(Result);
}

inline v2_s64 &
operator*=(v2_s64 &B, f64 A)
{
    B = A*B;

    return(B);
}

inline v2_s64
operator-(v2_s64 A)
{
    v2_s64 Result;

    Result.x = -A.x;
    Result.y = -A.y;

    return(Result);
}

inline v2_s64
operator+(v2_s64 A, v2_s64 B)
{
    v2_s64 Result;

#if MATH_SIMD
    Result.W = _mm_add_epi64(A.W, B.W);
#else
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
#endif
    return(Result);
}

inline v2_s64 &
operator+=(v2_s64 &A, v2_s64 B)
{
    A = A + B;

    return(A);
}

inline v2_s64
operator-(v2_s64 A, v2_s64 B)
{
    v2_s64 Result;

#if MATH_SIMD
    Result.W = _mm_sub_epi64(A.W, B.W);
#else
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
#endif
    return(Result);
}

inline v2_s64 &
operator-=(v2_s64 &A, v2_s64 B)
{
    A = A - B;

    return(A);
}

inline f64
Inner(v2_s64 A, v2_s64 B)
{
    f64 Result = Inner(V2F64(A), V2F64(B));

    return(Result);
}

inline f64
Inner(v2_s64 A, v2_s64 B, v2_s64 C)
{
    v2_f64 AB = V2F64(B - A);
    v2_f64 BC = V2F64(C - A);
    f64 Result = Inner(AB, BC);

    return(Result);
}

inline f64
Cross(v2_s64 A, v2_s64 B)
{
    f64 Result = Cross(V2F64(A), V2F64(B));

    return(Result);
}

inline f64
Cross(v2_s64 A, v2_s64 B, v2_s64 C)
{
    v2_f64 AB = V2F64(B - A);
    v2_f64 BC = V2F64(C - A);
    f64 Result = Cross(AB, BC);

    return(Result);
}

inline f64
LengthSq(v2_s64 A)
{
    f64 Result = Inner(A, A);

    return(Result);
}

inline f64
Length(v2_s64 A)
{
    f64 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline b32
PointsAreEqual(v2_s64 A, v2_s64 B)
{
    b32 Result = ((A.x == B.x) && (A.y == B.y));
    return(Result);
}

inline v2_s64
MidPointS64(v2_s64 p1, v2_s64 p2)
{
    v2_s64 Result;
    Result.x = (p1.x + p2.x) / 2;
    Result.y = (p1.y + p2.y) / 2;
    return(Result);
}

inline rectangle2i
InvertedInfinityRectangle2i(void)
{
    rectangle2i Result;

    Result.Min.x = Result.Min.y = INT64_MAX;
    Result.Max.x = Result.Max.y = -INT64_MAX;

    return(Result);
}

inline v2_s64
GetMinCorner(rectangle2i Rect)
{
    v2_s64 Result = Rect.Min;
    return(Result);
}

inline v2_s64
GetMaxCorner(rectangle2i Rect)
{
    v2_s64 Result = Rect.Max;
    return(Result);
}

inline v2_s64
GetDim(rectangle2i Rect)
{
    v2_s64 Result = Rect.Max - Rect.Min;
    return(Result);
}

inline v2_s64
GetCenter(rectangle2i Rect)
{
    v2_s64 Result = {};
    Result.x = (Rect.Min.x + Rect.Max.x) / 2;
    Result.y = (Rect.Min.y + Rect.Max.y) / 2;
    return(Result);
}

inline rectangle2i
RectMinMax(v2_s64 Min, v2_s64 Max)
{
    rectangle2i Result;

    Result.Min = Min;
    Result.Max = Max;

    return(Result);
}

inline rectangle2i
RectMinDim(v2_s64 Min, v2_s64 Dim)
{
    rectangle2i Result;

    Result.Min = Min;
    Result.Max = Min + Dim;

    return(Result);
}

inline rectangle2i
RectCenterHalfDim(v2_s64 Center, v2_s64 HalfDim)
{
    rectangle2i Result;

    Result.Min = Center - HalfDim;
    Result.Max = Center + HalfDim;

    return(Result);
}

inline rectangle2i
AddRadiusTo(rectangle2i A, v2_s64 Radius)
{
    rectangle2i Result;
    Result.Min = A.Min - Radius;
    Result.Max = A.Max + Radius;

    return(Result);
}

inline rectangle2i
ScaleRect(rectangle2i Rect, f64 Scale)
{
    rectangle2i Result = {};
    Result.Min = Scale*Rect.Min;
    Result.Max = Scale*Rect.Max;

    return(Result);
}

inline rectangle2i
Offset(rectangle2i A, v2_s64 Offset)
{
    rectangle2i Result;

    Result.Min = A.Min + Offset;
    Result.Max = A.Max + Offset;

    return(Result);
}

inline rectangle2i
RectCenterDim(v2_s64 Center, v2_s64 Dim)
{
    rectangle2i Result = RectCenterHalfDim(Center, 0.5f*Dim);

    return(Result);
}

inline b32
IsInRectangle(rectangle2i Rectangle, v2_s64 Test)
{
    bool32 Result = ((Test.x > Rectangle.Min.x) &&
                     (Test.y > Rectangle.Min.y) &&
                     (Test.x < Rectangle.Max.x) &&
                     (Test.y < Rectangle.Max.y));

    return(Result);
}

inline b32
IsInRectangle(rectangle2i Rect, rectangle2i Test)
{
    bool32 Result = ((Test.Min.x >= Rect.Min.x) &&
                     (Test.Min.y >= Rect.Min.y) &&
                     (Test.Max.x <= Rect.Max.x) &&
                     (Test.Max.y <= Rect.Max.y));

    return(Result);
}

inline b32
Intersects(rectangle2i A, rectangle2i B)
{
    b32 Result = ((Maximum(A.Min.x, B.Min.x) <= Minimum(A.Max.x, B.Max.x)) &&
                  (Maximum(A.Max.y, B.Max.y) <= Minimum(A.Min.y, B.Min.y)));

    return(Result);
}

inline b32
IsRectEmpty(rectangle2i Rect)
{
    b32 Result = ((Rect.Min.y <= Rect.Max.y) ||
                  (Rect.Max.x <= Rect.Min.x));

    return(Result);
}

inline b32
RectsAreEqual(rectangle2i A, rectangle2i B)
{
    b32 Result = (PointsAreEqual(A.Min, B.Min) &&
                  PointsAreEqual(A.Max, B.Max));
    return(Result);
}

inline rectangle2i
Union(rectangle2i A, rectangle2i B)
{
    rectangle2i Result;
    
    Result.Min.x = Minimum(A.Min.x, B.Min.x);
    Result.Min.y = Minimum(A.Min.y, B.Min.y);
    Result.Max.x = Maximum(A.Max.x, B.Max.x);
    Result.Max.y = Maximum(A.Max.y, B.Max.y);

    return(Result);
}

inline rectangle2i
GetBounds(path_s64 *Path)
{
    rectangle2i Result = InvertedInfinityRectangle2i();

    for(s32 I = 0;
        I < Path->Count;
        ++I)
    {
        v2_s64 p = Path->Points[I];
        if (p.x < Result.Min.x) Result.Min.x = p.x;
        if (p.x > Result.Max.x) Result.Max.x = p.x;
        if (p.y < Result.Min.y) Result.Min.y = p.y;
        if (p.y > Result.Max.y) Result.Max.y = p.y;
    }

    return(Result);
}

inline rectangle2i
GetBounds(paths_s64 *Paths)
{
    rectangle2i Result = InvertedInfinityRectangle2i();

    Result = GetBounds(Paths->Paths + 0);
    for(s32 I = 1;
        I < Paths->PathCount;
        ++I)
    {
        path_s64 *Path = Paths->Paths + I;
        rectangle2i Bounds = GetBounds(Path);
        Result = Union(Result, Bounds);
    }

    return(Result);
}


inline b32
NearEqual(v2_s64 p1, v2_s64 p2, f64 MaxDistSq)
{
    v2_s64 p21 = p1 - p2;

    b32 Result = (Square(p21.x) + Square(p21.y) < MaxDistSq);
    return(Result);
}


inline f64
PerpDistFromLineSq(v2_s64 p, v2_s64 a, v2_s64 b)
{
    TimeFunction;

    // a = p.x - a.x : ap.x
    // b = b.x - a.x : ab.x
    // c = p.y - a.y : ap.y
    // d = b.y - a.y : ab.y

    // c = a*d - c*b
    // r = c^2 / b*b + d*d
    // r = (a*d - c*b)^2 / b*b + d*d
    // r = (a*d - c*b)^2 / b^2 + d^2

    //      (a*d)^2 - 2*(a*d)*(c*b) + (c*b)^2
    // r =  _________________________________
    //                 b^2 + d^2

    // sub(a, b, c, d) = _mm256_sub_pd(set(p.x, b.x, p.y, b.y), set(a.x, a.x, a.y, a.y))

    // s[4] = mul(ad(x), bb(z), bc(y), dd(w)) = _mm256_mul_pd(set(a, b, c, d), set(d, b, b, d))
    
    //          (x - y)^2
    // r =     _____________
    //           z + w
    
    // r = (s[0] - s[2])^2 / (s[1] + s[3]);
    f64 Result = 0.0;
#if 1
    v2_f64 ap = V2F64(p - a);
    v2_f64 ab = V2F64(b - a);
    
    if(!((ab.x == 0) && (ab.y == 0)))
    {
        Result = Square(Cross(ap, ab)) / Inner(ab, ab);
    }
#else
    f64 buf[4] = {};
    __m256d Sub = _mm256_sub_pd(_mm256_set_pd((f64)p.x, (f64)b.x, (f64)p.y, (f64)b.y),
                                _mm256_set_pd((f64)a.x, (f64)a.x, (f64)a.y, (f64)a.y));

    _mm256_store_pd(buf, Sub);

    if(!((buf[0] == 0) && (buf[2] == 0)))    
    {
        __m256d Sub1 = _mm256_set_pd(buf[0], buf[2], buf[2], buf[0]);
        __m256d Mul = _mm256_mul_pd(Sub, Sub1);
        _mm256_store_pd(buf, Mul);

        Result = Square(buf[3] - buf[1]) / (buf[2] + buf[0]);
    }
#endif    

    return(Result);
}

inline f64
Area(path_s64 *Path)
{
    f64 Result = 0.0;

    s32 Count = Path->Count;
    if(Count >= 3)
    {
        v2_s64 *Iter1 = Path->Points + (Count - 2);
        v2_s64 *Iter2 = Iter1;
        v2_s64 *Stop = Iter2;

        if(!(Count & 1))
        {
            ++Stop;
        }

        for(Iter1 = Path->Points;
            !PointsAreEqual(*Iter1, *Stop);)
        {
            Result += (f64)((Iter2->y + Iter1->y) * (Iter2->x - Iter1->x));
            Iter2 = Iter1 + 1;
            Result += (f64)((Iter1->y + Iter2->y) * (Iter1->x - Iter2->x));
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
Area(paths_s64 *Paths)
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
IsPositive(path_s64 *Poly)
{
    // A curve has positive orientation [and area] if a region 'R'
    // is on the left when traveling around the outside of 'R'.
    //https://mathworld.wolfram.com/CurveOrientation.html
    //nb: This statement is premised on using Cartesian coordinates
    b32 Result = (Area(Poly) >= 0);
    return(Result);
}

inline b32
IsCollinear(v2_s64 p1, v2_s64 SharedP, v2_s64 p2) // #777
{
    s64 a = SharedP.x - p1.x;
    s64 b = p2.y - SharedP.y;
    s64 c = SharedP.y - p1.y;
    s64 d = p2.x - SharedP.x;
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
GetSegmentIntersectPt(v2_s64 a, v2_s64 b, v2_s64 c, v2_s64 d, v2_s64 *p)
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

        // TODO(babykaban): The problem might be here
        p->x = originx + hitx;
        p->y = originy + hity;

        Result = true;
    }

    return(Result);
}

#else

inline b32
GetSegmentIntersectPt(v2_s64 a, v2_s64 b, v2_s64 c, v2_s64 d, v2_s64 *p)
{
    // https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection

    b32 Result = false;
    
    v2_f64 ab = V2F64(b - a);
    v2_f64 cd = V2F64(d - c);

//    f64 det = Cross(cd, ab);
    f64 det = cd.x*ab.y - cd.y*ab.x;
    if(det != 0.0)
    {
        f64 t = ((a.x - c.x) * cd.y - (a.y - c.y) * cd.x) / det;

        if(t <= 0.0)
            *p = a;
        else if(t >= 1.0)
            *p = b;
        else
        {
            *p = {(s64)(a.x + t*ab.x), (s64)(a.y + t*ab.y)};
        }
    }

    return true;
}
#endif

inline v2_s64
TranslatePoint(v2_s64 p, v2_s64 Offset)
{
    // TODO(babykaban): Eliminate
    v2_s64 Result = p + Offset;
    return(Result);
}

inline v2_s64
ReflectPoint(v2_s64 p, v2_s64 pivot)
{
    v2_s64 Result = pivot + (pivot - p);
    return(Result);
}

inline s32
GetSign(s64 Val) 
{ 
    s32 Result = 0;
    if(Val)
    {
        Result =  (Val > 0) ? 1 : -1;
    }

    return(Result);
}

inline bool
SegmentsIntersect(v2_s64 seg1a, v2_s64 seg1b, v2_s64 seg2a, v2_s64 seg2b, b32 inclusive = false)
{
    b32 Result = false;

    if(inclusive)
    {
        f64 res1 = Cross(seg1a, seg2a, seg2b);
        f64 res2 = Cross(seg1b, seg2a, seg2b);
        if((res1 * res2) <= 0)
        {
            f64 res3 = Cross(seg2a, seg1a, seg1b);
            f64 res4 = Cross(seg2b, seg1a, seg1b);
            if ((res3 * res4) <= 0)
            {
                Result = (res1 || res2 || res3 || res4); // ensures not collinear
            }
        }
    }
    else
    {
        Result = ((GetSign(Cross(seg1a, seg2a, seg2b)) * GetSign(Cross(seg1b, seg2a, seg2b)) < 0) &&
                  (GetSign(Cross(seg2a, seg1a, seg1b)) * GetSign(Cross(seg2b, seg1a, seg1b)) < 0));
    }

    return(Result);
}

inline v2_s64
GetClosestPointOnSegment(v2_s64 offPt, v2_s64 seg1, v2_s64 seg2)
{
    v2_s64 Result = {};
    
    if(PointsAreEqual(seg1, seg2))
    {
        Result = seg1;
    }
    else
    {
        v2_f64 d = V2F64(seg2 - seg1);

        f64 q = Inner(V2F64(offPt - seg1), d) / Inner(d, d);
        Clamp01(q);

        Result = seg1 + V2S64(q*d);
    }

    return(Result);
}


inline point_in_poly_res
PointInPolygon(v2_s64 p, path_s64 *Poly)
{
    point_in_poly_res Result = PointInPolyRes_Outside;

    s32 val = 0;
    if(Poly->Count >= 3)
    {
        v2_s64 *Start = Poly->Points + 0;
        v2_s64 *End = Poly->Points + (Poly->Count - 1);
        v2_s64 *First = Start;
        v2_s64 *Cur;
        v2_s64 *Prev;

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

#define CLIPPER_MATH_S64_H
#endif
