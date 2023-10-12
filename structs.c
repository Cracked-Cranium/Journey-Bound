#include "raylib.h"

// -----------------------------------------
// Short2 struct and functions
// -----------------------------------------

// Like Vector2, but with shorts
typedef struct Short2
{
    short x;
    short y;
} Short2;

// Returns the scaled vector (multiplies both values)
Short2 Short2Scale(Short2 in, short scale)
{
    return (Short2){
        in.x * scale,
        in.y * scale};
}

// Divides both values and returns result
Short2 Short2Divide(Short2 in, short divisor)
{
    return (Short2){
        in.x / divisor,
        in.y / divisor};
}

Short2 Short2Add(Short2 in1, Short2 in2)
{
    return (Short2){
        in1.x + in2.x,
        in1.y + in2.y};
}

// -----------------------------------------
// OBJECTS
// -----------------------------------------

typedef struct Item
{
    unsigned short data;
} Item;

typedef struct Object
{
    unsigned short texture_id;
    short health, x, y;
} Object;

typedef struct DynObject
{
    unsigned short texture_id;
    short health;
    Item primitve_inventory[8];
    Vector2 pos;
} DynObject;

typedef struct Player
{
    DynObject core;
    Item *main_inventory;
    unsigned char equipment[8];
} Player;

// -----------------------------------------
// WORLD / MAP
// -----------------------------------------

typedef struct Tile
{
    short base_texture_id;
    short occupant_texture_id; // ?
    bool solid;
} Tile;

typedef struct Chunk
{
    short tiles[CHUNK_SIZE_TILES * CHUNK_SIZE_TILES];
    Short2 chunk_pos;
    // Object *objs;
    // DynObject *dyn_objs;
} Chunk;

// -----------------------------------------
// Dynamic chunk array and functions
// -----------------------------------------

typedef struct DynChunkArr
{
    Chunk *chunks;
    int capacity;
    int count;
} DynChunkArr;

void InitDCA(DynChunkArr *arr, int capacity)
{
    (*arr).count = 0;
    (*arr).capacity = capacity;
    (*arr).chunks = (Chunk *)malloc(sizeof(Chunk) * (*arr).capacity);
}

void AddDCA(DynChunkArr *arr, Chunk chunk)
{
    if ((*arr).count == (*arr).capacity)
    {
        (*arr).capacity *= 2;
        (*arr).chunks = (Chunk *)realloc((*arr).chunks, sizeof(Chunk) * (*arr).capacity);
    }

    (*arr).chunks[(*arr).count] = chunk;
    (*arr).count++;
}