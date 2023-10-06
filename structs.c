#include "raylib.h"

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
    short chunk_x;
    short chunk_y;
    //Object *objs;
    //DynObject *dyn_objs;
} Chunk;

