#include "raylib.h"
#include "enums.c"
#include "structs.c"

#define TILE_SIZE_PIXELS (short)16 // 16 * 16
#define CHUNK_SIZE 256 // 16 * 16
#define WINDOW_WIDTH GetScreenWidth
#define WINDOW_HEIGHT GetScreenHeight

Texture GameSprites;

int load()
{
    GameSprites = LoadTexture("sprites/spritesheet.png");
}

int main()
{
    Tile map[CHUNK_SIZE * 6];

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Journey Bound");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);

        EndDrawing();
    }

    return 0;
}