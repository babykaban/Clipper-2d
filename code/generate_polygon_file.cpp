/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */
#include "main.h"
#include <time.h>

#include "clipper_math.h"
#include "clipper_math_f64.h"

#define PROFILER 1
#include "profiler.cpp"
#include "pcg_random.cpp"

#if 0 
union triangle
{
    struct
    {
        v2_f64 p1;
        v2_f64 p2;
        v2_f64 p3;
    };

    v2_f64 Verts[3];
};

inline b32
IsCollinear(v2_f64 a, v2_f64 b, v2_f64 c, f64 Epsilon)
{
    f64 Area = fabs((b.y - a.y) * (c.x - b.x) - (c.y - b.y) * (b.x - a.x));
    return(Area < Epsilon);
}

// Function to generate a random float between min and max
inline f64
RandFloat(f64 min, f64 max)
{
    return(min + (f64)rand() / (f64)(RAND_MAX / (max - min)));
}

internal triangle
GenerateRandomTriangle(f64 minX, f64 maxX, f64 minY, f64 maxY, f64 Epsilon)
{
    triangle Result = {};

    do
    {
        // Generate random points within the specified range
        Result.p1.x = RandFloat(minX, maxX);
        Result.p1.y = RandFloat(minY, maxY);

        Result.p2.x = RandFloat(minX, maxX);
        Result.p2.y = RandFloat(minY, maxY);

        Result.p3.x = RandFloat(minX, maxX);
        Result.p3.y = RandFloat(minY, maxY);

    } while (IsCollinear(Result.p1, Result.p2, Result.p3, Epsilon)); // Repeat if points are nearly collinear

    return(Result);
}

inline f64
ProjectPointOnAxis(v2_f64 p, v2_f64 axis)
{
    f64 Result = Inner(p, axis) / SquareRoot(Inner(axis, axis));
    return(Result);
}

// Get the min and max projections of a triangle on an axis
inline void
ProjectTriangleOnAxis(triangle T, v2_f64 axis, f64 *min, f64 *max)
{
    *min = *max = ProjectPointOnAxis(T.p1, axis);

    for(u32 i = 1; i < 3; ++i)
    {
        f64 projection = ProjectPointOnAxis(T.Verts[i], axis);
        if(projection < *min)
            *min = projection;
        if(projection > *max)
            *max = projection;
    }
}

// Check if projections overlap on a given axis
inline b32
ProjectionsOverlap(f64 minA, f64 maxA, f64 minB, f64 maxB)
{
    return !(maxA < minB || maxB < minA);
}

// Check if two triangles overlap using the Separating Axis Theorem
b32
TrianglesOverlap(triangle T0, triangle T1)
{
    // Get the edges of each triangle
    v2_f64 edges0[3] = { T0.p2 - T0.p1, T0.p3 - T0.p2, T0.p1 - T0.p3 };
    v2_f64 edges1[3] = { T1.p2 - T1.p1, T1.p3 - T1.p2, T1.p1 - T1.p3 };

    // Test each edge as a potential separating axis
    for(int i = 0; i < 3; ++i)
    {
        v2_f64 axis1 = { -edges0[i].y, edges0[i].x }; // Perpendicular to edge
        v2_f64 axis2 = { -edges1[i].y, edges1[i].x }; // Perpendicular to edge

        // Project both triangles onto the axis
        f64 min1, max1, min2, max2;

        ProjectTriangleOnAxis(T0, axis1, &min1, &max1);
        ProjectTriangleOnAxis(T1, axis1, &min2, &max2);

        if(!ProjectionsOverlap(min1, max1, min2, max2))
            return 0; // Found a separating axis

        ProjectTriangleOnAxis(T0, axis2, &min1, &max1);
        ProjectTriangleOnAxis(T1, axis2, &min2, &max2);

        if(!ProjectionsOverlap(min1, max1, min2, max2))
            return 0; // Found a separating axis
    }

    // If no separating axis was found, triangles overlap
    return 1;
}

internal triangle
GenerateOverlapTriangleFor(triangle T, f64 minX, f64 maxX, f64 minY, f64 maxY, f64 Epsilon)
{
    triangle Result = {};

    do
    {
        Result = GenerateRandomTriangle(minX, maxX, minY, maxY, Epsilon);

    } while(!TrianglesOverlap(T, Result));

    return(Result);
}

inline void
PrintTriangle(triangle T)
{
    printf("Triangle is: ");

    for(u32 I = 0;
        I < 3;
        ++I)
    {
        printf("(%.2f, %.2f), ", T.Verts[I].x, T.Verts[I].y);
    }

    printf("\n");
}
#endif
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Define a struct for a 2D point
typedef struct {
  double x;
  double y;
} Point;

// Function to generate a random point within a given bounding box
Point randomPoint(double minX, double maxX, double minY, double maxY) {
  Point p;
  p.x = minX + (rand() / (double)RAND_MAX) * (maxX - minX);
  p.y = minY + (rand() / (double)RAND_MAX) * (maxY - minY);
  return p;
}

// Function to calculate the orientation of three points (p, q, r)
// Returns 0 if collinear, 1 if clockwise, 2 if counterclockwise
int orientation(Point p, Point q, Point r) {
  double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
  if (val == 0) return 0;
  return (val > 0) ? 1 : 2;
}

// Function to check if point r lies on line segment pq
int onSegment(Point p, Point r, Point q) {
  if (r.x <= fmax(p.x, q.x) && r.x >= fmin(p.x, q.x) &&
      r.y <= fmax(p.y, q.y) && r.y >= fmin(p.y, q.y))
    return 1;
  return 0;
}

// Function to check if line segments p1q1 and p2q2 intersect
int doIntersect(Point p1, Point q1, Point p2, Point q2) {
  int o1 = orientation(p1, q1, p2);
  int o2 = orientation(p1, q1, q2);
  int o3 = orientation(p2, q2, p1);
  int o4 = orientation(p2, q2, q1);

  if (o1 != o2 && o3 != o4) return 1;

  // Special Cases (collinear)
  if (o1 == 0 && onSegment(p1, p2, q1)) return 1;
  if (o2 == 0 && onSegment(p1, q2, q1)) return 1;
  if (o3 == 0 && onSegment(p2, p1, q2)) return 1;
  if (o4 == 0 && onSegment(p2, q1, q2)) return 1;

  return 0;
}

// Comparison function for qsort to sort points by angle
int comparePoints(const void* a, const void* b) {
  Point *p1 = (Point *)a;
  Point *p2 = (Point *)b;

  if (p1->x < p2->x) return -1;
  if (p1->x > p2->x) return 1;
  return 0; 
}
// Function to generate a random simple polygon with a given number of vertices
// within a bounding box
Point* generateRandomPolygon(int numVertices, double minX, double maxX, double minY, double maxY) {
  // Allocate memory for the polygon vertices
  Point* polygon = (Point*)malloc(numVertices * sizeof(Point));
  if (polygon == NULL) {
    return NULL; // Memory allocation failed
  }

  // Generate random points
  for (int i = 0; i < numVertices; i++) {
    polygon[i] = randomPoint(minX, maxX, minY, maxY);
  }

  // Sort points in counter-clockwise order using a simple wrapping algorithm
  // This helps avoid self-intersections
  Point center = {0, 0};
  for (int i = 0; i < numVertices; i++) {
    center.x += polygon[i].x;
    center.y += polygon[i].y;
  }
  center.x /= numVertices;
  center.y /= numVertices;

  // Calculate angle for each point relative to the center
  for (int i = 0; i < numVertices; i++) {
    double angle = atan2(polygon[i].y - center.y, polygon[i].x - center.x);
    polygon[i].x = angle; // Temporarily store angle in x for sorting
  }

  // Sort points by angle
  qsort(polygon, numVertices, sizeof(Point), comparePoints);

  // Restore original x values
  for (int i = 0; i < numVertices; i++) {
    double angle = polygon[i].x;
    polygon[i].x = center.x + cos(angle) * (rand() / (double)RAND_MAX) * (maxX - minX) / 2;
  }

  // Check for self-intersections and regenerate if found
  int hasIntersection = 0;
  for (int i = 0; i < numVertices - 1; i++) {
    for (int j = i + 2; j < numVertices; j++) {
      // Don't check adjacent edges
      if (i == 0 && j == numVertices - 1) continue;

      if (doIntersect(polygon[i], polygon[i + 1], polygon[j], polygon[(j + 1) % numVertices])) {
        hasIntersection = 1;
        break;
      }
    }
    if (hasIntersection) break;
  }

  if (hasIntersection) {
    // Free the current polygon and try generating a new one
    free(polygon);
    return generateRandomPolygon(numVertices, minX, maxX, minY, maxY);
  }

  return polygon;
}

int main() {
  // Seed the random number generator
    srand((u32)time(0));

    // Generate a random polygon with 10 vertices
    for(int k = 0;
        k < 5;
        ++k)
    {
        int numVertices = 20;
        Point* polygon = generateRandomPolygon(numVertices, -100, 100, -100, 100);
 
        if (polygon != NULL) {
            // Print the polygon vertices
            for (int i = 0; i < numVertices; i++) {
//      printf("Vertex %d: (%f, %f)\n", i, polygon[i].x, polygon[i].y);
                printf("(%f, %f), ", polygon[i].x, polygon[i].y);
            }

            printf("\n\n");

            // Free the allocated memory
            free(polygon);
        }
    }
    
  return 0;
}
#if 0
int
main()
{
    BeginProfile();

    srand((u32)time(0)); // Seed for randomness
    
    u32 Count = 48000000;
    triangle *Tris = (triangle *)malloc(sizeof(triangle)*Count);
    for(u32 I = 0;
        I < Count;
        I += 2)
    {
        Tris[I] = GenerateRandomTriangle(-200.0, 200.0, -200.0, 200.0, 5.0);
        Tris[I + 1] = GenerateOverlapTriangleFor(Tris[I], -200.0, 200.0, -200.0, 200.0, 5.0);
        PrintTriangle(Tris[I]);
        PrintTriangle(Tris[I + 1]);
    }

//    triangle T = GenerateRandomTriangle(-100.0, 100.0, -100.0, 100.0, 3.0);
//    PrintTriangle(T);

//    triangle T0 = GenerateOverlapTriangleFor(T, -100.0, 100.0, -100.0, 100.0, 3.0);
//    PrintTriangle(T0);

    EndAndPrintProfile();
    
    return(0);
}

#endif
