#if !defined(GENERATE_POLYGON_FILE_H)
/* ========================================================================
   $File: $
   $Date: 2024 $
   $Revision: $
   $Creator: BabyKaban $
   $Notice: $
   ======================================================================== */

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

struct polygon
{
    u32 Count;
    v2_f64 *Points;
};

struct polygon_set
{
    u32 PolyCount;
    polygon *Polygons;
};

// Function to generate a random float between min and max
inline f64
RandDouble(f64 min, f64 max)
{
    TimeFunction;

    f64 Result = min + (f64)rand() / (f64)(RAND_MAX / (max - min));
    return(Result);
}

inline __m128d
RandDouble_2x(f64 min, f64 max)
{
    TimeFunction;

    f64 D = RAND_MAX / (max - min);
    f64 DInv = 1.0 / D;
    
    __m128d Min_2x = _mm_set1_pd(min);
    __m128d Rand_2x = _mm_mul_pd(_mm_set_pd((f64)rand(), (f64)rand()), _mm_set1_pd(DInv));

    __m128d Result = _mm_add_pd(Min_2x, Rand_2x);

    return(Result);
}

inline __m256d
RandDouble_4x(f64 min, f64 max)
{
    TimeFunction;

    f64 D = RAND_MAX / (max - min);
    f64 DInv = 1.0 / D;
    
    __m256d Min_4x = _mm256_set1_pd(min);
    __m256d Rand_4x = _mm256_mul_pd(_mm256_set_pd((f64)rand(), (f64)rand(), (f64)rand(), (f64)rand()),
                                    _mm256_set1_pd(DInv));

    __m256d Result = _mm256_add_pd(Min_4x, Rand_4x);

    return(Result);
}

inline __m256d
RandDouble1_4x(void)
{
    TimeFunction;

    f64 DInv = 1.0 / RAND_MAX;
    __m256d Rand_4x = _mm256_set_pd((f64)rand(), (f64)rand(), (f64)rand(), (f64)rand());

    __m256d Result = _mm256_mul_pd(Rand_4x, _mm256_set1_pd(DInv));

    return(Result);
}

inline __m128d
RandDouble1_2x(void)
{
    TimeFunction;

    f64 DInv = 1.0 / RAND_MAX;
    __m128d Rand_2x = _mm_set_pd((f64)rand(), (f64)rand());

    __m128d Result = _mm_mul_pd(Rand_2x, _mm_set1_pd(DInv));

    return(Result);
}

inline f64
RandDouble1(void)
{
    TimeFunction;

    f64 DInv = 1.0 / RAND_MAX;
    f64 Rand = rand();

    f64 Result = Rand*DInv;

    return(Result);
}

// Function to generate a random point within a given bounding box
inline v2_f64
RandomPoint(f64 minX, f64 maxX, f64 minY, f64 maxY)
{
    v2_f64 Result = {};

    Result.x = RandDouble(minX, maxX);
    Result.y = RandDouble(minY, maxY);

    return(Result);
}

#define TABLE_SIZE 1000

// Structure to represent a key-value pair in the hash table
struct ht_item
{
    f64 key;
    u32 value;

    ht_item *next;
};

// Structure to represent the hash table
struct ht
{
    u32 size;
    ht_item** items;
};

// Hash function for double keys
inline u32
hash_function(f64 key)
{
    // Convert the double to an integer representation
    s64 int_key = *(s64*)&key;

    // Simple hash function using modulo operator
    return abs(int_key) % TABLE_SIZE;
}

// Create a new hash table
internal ht *
ht_create(void)
{
    ht *table = (ht *)malloc(sizeof(ht));
    table->size = TABLE_SIZE;
    table->items = (ht_item **)calloc(TABLE_SIZE, sizeof(ht_item*));
    return table;
}

// Insert a key-value pair into the hash table
inline void
ht_insert(ht *table, f64 key, u32 value)
{
    u32 index = hash_function(key);

    // Handle collisions using separate chaining (linked list)
    ht_item *item = (ht_item*)malloc(sizeof(ht_item));
    item->key = key;
    item->value = value;
    item->next = table->items[index];
    table->items[index] = item;
}

// Get the value associated with a key from the hash table
inline s32
ht_get(ht *table, f64 key)
{
    u32 index = hash_function(key);
    ht_item *item = table->items[index];

    // Traverse the linked list to find the key
    while(item)
    {
        if(item->key == key)
        {
            return item->value;
        }

        item = item->next;
    }

    return -1; // Key not found
}

// Delete a key-value pair from the hash table
inline void
ht_delete(ht *table, f64 key)
{
    u32 index = hash_function(key);
    ht_item *item = table->items[index];
    ht_item *prev = 0;

    // Traverse the linked list to find the key
    while(item)
    {
        if(item->key == key)
        {
            if(!prev)
            {
                table->items[index] = item->next;
            }
            else
            {
                prev->next = item->next;
            }

            free(item);
            return;
        }

        prev = item;
        item = item->next;
    }
}

// Free the memory allocated for the hash table
internal void
ht_destroy(ht *table)
{
    for(u32 i = 0; i < table->size; i++)
    {
        ht_item *item = table->items[i];
        while(item)
        {
            ht_item *next = item->next;

            free(item);
            item = next;
        }
    }

    free(table->items);
    free(table);
}

#define GENERATE_POLYGON_FILE_H
#endif
