/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */

#include "main.h"

#include "clipper.h"
#include "clipper.cpp"

int main()
{

    paths_s64 Subject = GetPathsS64(3);
    Subject.PathCount = 3;

    paths_s64 Clip = GetPathsS64(1);
    Clip.PathCount = 1;

    Subject.Paths[0] = GetPathS64(4);
    Subject.Paths[0].Count = 4;
    Subject.Paths[0].Points[0] = {50, 150};
    Subject.Paths[0].Points[1] = {200, 150};
    Subject.Paths[0].Points[2] = {200, 300};
    Subject.Paths[0].Points[3] = {50, 300};

    Subject.Paths[1] = GetPathS64(4);
    Subject.Paths[1].Count = 4;
    Subject.Paths[1].Points[0] = {60, 160};
    Subject.Paths[1].Points[1] = {190, 160};
    Subject.Paths[1].Points[2] = {190, 290};
    Subject.Paths[1].Points[3] = {60, 290};

    Subject.Paths[2] = GetPathS64(4);
    Subject.Paths[2].Count = 4;
    Subject.Paths[2].Points[0] = {70, 170};
    Subject.Paths[2].Points[1] = {180, 170};
    Subject.Paths[2].Points[2] = {180, 280};
    Subject.Paths[2].Points[3] = {70, 280};

    Clip.Paths[0] = GetPathS64(4);
    Clip.Paths[0].Count = 4;
    Clip.Paths[0].Points[0] = {100, 100};
    Clip.Paths[0].Points[1] = {250, 100};
    Clip.Paths[0].Points[2] = {250, 250};
    Clip.Paths[0].Points[3] = {100, 250};

    paths_s64 Result = BooleanOp(ClipType_Intersection, FillRule_EvenOdd, &Subject, &Clip);

    printf("Intersection: ");
    for(s32 I = 0;
        I < Result.PathCount;
        ++I)
    {
        path_s64 *Path = Result.Paths + I;
        for(s32 J = 0;
            J < (Path->Count - 1);
            ++J)
        {
            if((J % 4) == 0)
            {
                printf("\n");
            }

            printf("{%llu, %llu}, ", Path->Points[J].x, Path->Points[J].y);
        }

        printf("{%llu, %llu};", Path->Points[Path->Count - 1].x, Path->Points[Path->Count - 1].y);
    }

    printf("\n\n");

    return(0);
}
