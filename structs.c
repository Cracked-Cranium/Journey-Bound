#include "raylib.h"

typedef struct Tile
{
    short base_texture_id;
    short occupant_texture_id;
    bool solid;
} Tile;

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