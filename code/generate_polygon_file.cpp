/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice:  $
   ======================================================================== */
#if 1
#include "main.h"
#include <time.h>

enum polygon_type
{
    PolygonType_Convex,
    PolygonType_Concave,
    PolygonType_WithHole,
};

struct point
{
    f64 x, y;
};


// Function to generate a random float between min and max
inline f64
RandFloat(f64 min, f64 max)
{
    return(min + (f64)rand() / (f64)(RAND_MAX / (max - min)));
}

// Function to check if three points make a right turn (cross product)
inline s32
is_right_turn(point a, point b, point c)
{
    f64 cross_product = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    return cross_product > 0;
}

internal point *
GenerateConvexPolygon(u32 n, f64 min_x, f64 max_x, f64 min_y, f64 max_y, u32 *actual_size)
{
    if(n < 3)
    {
        printf("Error: Polygon must have at least 3 vertices\n");
        return NULL;
    }

    // Allocate memory for points
    point *points = (point *)malloc(n * sizeof(point));

    if(!points)
    {
        printf("Error: Memory allocation failed\n");
        return NULL;
    }

    // Generate random points in a circle to ensure convexity
    f64 center_x = (min_x + max_x) / 2;
    f64 center_y = (min_y + max_y) / 2;
    f64 radius = fmin(max_x - min_x, max_y - min_y) / 2;

    for(u32 i = 0; i < n; i++)
    {
        f64 angle = RandFloat(0, 2 * Pi32);
        f64 r = radius * RandFloat(0.5, 1.0); // Varying radius for more natural look
        
        points[i].x = center_x + r * cos(angle);
        points[i].y = center_y + r * sin(angle);
    }

    // Sort points by polar angle relative to center point
    point center = {center_x, center_y};
    
    // Bubble sort by polar angle (could be optimized with quicksort)
    for(u32 i = 0; i < n - 1; i++)
    {
        for (u32 j = 0; j < n - i - 1; j++)
        {
            f64 angle1 = atan2(points[j].y - center.y, points[j].x - center.x);
            f64 angle2 = atan2(points[j + 1].y - center.y, points[j + 1].x - center.x);
            
            if(angle1 > angle2)
            {
                point temp = points[j];
                points[j] = points[j+1];
                points[j+1] = temp;
            }
        }
    }

    *actual_size = n;

    return points;
}

internal void
print_polygon(point *points, int n)
{
    printf("Convex Polygon Points:\n");
    for (int i = 0; i < n; i++)
    {
        printf("(%.2f, %.2f),\n", points[i].x, points[i].y);
    }
}

int
main()
{
    srand((u32)time(0));

    u32 n = 256;  // Number of points
    u32 actual_size = 0;

    point *Points = GenerateConvexPolygon(n, -1000, 1000, -1000, 1000, &actual_size);
    print_polygon(Points, n);
    printf("Actual Size: %u\n", actual_size);

#if 0 

    Points = GenerateConvexPolygon(n, -40, 40, -50, 50, &actual_size);
    print_polygon(Points, n);
    printf("Actual Size: %u\n", actual_size);

    Points = GenerateConvexPolygon(n, -40, 40, -50, 50, &actual_size);
    print_polygon(Points, n);
    printf("Actual Size: %u\n", actual_size);
#endif

    return(0);
}
#endif

#if 0

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct {
    double x;
    double y;
} Point;

// Global reference point for sorting
Point p0;

// Function to generate a random number between min and max
double random_range(double min, double max) {
    return min + (rand() / (double)RAND_MAX) * (max - min);
}

// Function to calculate square of distance between two points
double dist_sq(Point p1, Point p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

// Function to find orientation of triplet (p, q, r)
// Returns:
//  0 --> Collinear
//  1 --> Clockwise
//  2 --> Counterclockwise
int orientation(Point p, Point q, Point r) {
    double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (fabs(val) < 1e-10) return 0;  // collinear
    return (val > 0) ? 1 : 2;
}

// Function to find the bottom-most point
int find_bottom_point(Point* points, int n) {
    int bottom = 0;
    for (int i = 1; i < n; i++) {
        if (points[i].y < points[bottom].y ||
            (points[i].y == points[bottom].y && points[i].x < points[bottom].x)) {
            bottom = i;
        }
    }
    return bottom;
}

// Compare function for qsort
int compare(const void* vp1, const void* vp2) {
    Point* p1 = (Point*)vp1;
    Point* p2 = (Point*)vp2;
    
    int o = orientation(p0, *p1, *p2);
    if (o == 0) {
        return (dist_sq(p0, *p2) >= dist_sq(p0, *p1)) ? -1 : 1;
    }
    return (o == 2) ? -1 : 1;
}

// Function to generate vertices of a convex polygon using Graham's scan
Point* generate_convex_polygon(int n, double min_x, double max_x, double min_y, double max_y, int* actual_size) {
    if (n < 3) {
        printf("Error: Polygon must have at least 3 vertices\n");
        return NULL;
    }

    // Generate more points than needed to ensure we get enough after convex hull
    int extra_points = n * 3;
    Point* points = (Point*)malloc(extra_points * sizeof(Point));
    if (!points) {
        printf("Error: Memory allocation failed\n");
        return NULL;
    }

    // Generate random points
    for (int i = 0; i < extra_points; i++) {
        points[i].x = random_range(min_x, max_x);
        points[i].y = random_range(min_y, max_y);
    }

    // Find the bottom-most point
    int bottom = find_bottom_point(points, extra_points);
    
    // Swap bottom point with first point
    Point temp = points[0];
    points[0] = points[bottom];
    points[bottom] = temp;

    // Set global reference point for sorting
    p0 = points[0];

    // Sort points by polar angle with respect to bottom point
    qsort(&points[1], extra_points - 1, sizeof(Point), compare);

    // Initialize stack for Graham's scan
    Point* stack = (Point*)malloc(extra_points * sizeof(Point));
    int stack_size = 0;

    // Push first three points to stack
    stack[stack_size++] = points[0];
    stack[stack_size++] = points[1];
    stack[stack_size++] = points[2];

    // Process remaining points
    for (int i = 3; i < extra_points; i++) {
        while (stack_size > 1 && orientation(stack[stack_size - 2], 
                                          stack[stack_size - 1], 
                                          points[i]) != 2) {
            stack_size--;
        }
        stack[stack_size++] = points[i];
    }

    // Check if we have enough points for desired polygon
    if (stack_size < n) {
        printf("Warning: Could only generate %d vertices (requested %d)\n", 
               stack_size, n);
        *actual_size = stack_size;
    } else {
        *actual_size = n;
    }

    // Create result array with exactly n points (or stack_size if smaller)
    Point* result = (Point*)malloc(*actual_size * sizeof(Point));
    for (int i = 0; i < *actual_size; i++) {
        result[i] = stack[i];
    }

    // Clean up
    free(points);
    free(stack);

    return result;
}

// Example usage
int main() {
    srand((unsigned int)time(NULL));
    
    int n = 100;  // Number of desired vertices
    int actual_size;
    Point* polygon = generate_convex_polygon(n, -100, 100, -100, 100, &actual_size);
    
    if (polygon) {
        printf("Generated convex polygon vertices:\n");
        for (int i = 0; i < actual_size; i++) {
            printf("(%.2f, %.2f),\n", polygon[i].x, polygon[i].y);
        }
        
        free(polygon);
    }
    
    return 0;
}
#endif
