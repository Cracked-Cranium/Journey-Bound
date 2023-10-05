#include "raylib.h"
#include "raymath.h"

#include "enums.c"
#include "structs.c"

#define TILE_SIZE_PIXELS (short)16
#define RENDER_WIDTH_TILES (short)18
#define RENDER_HEIGHT_TILES (short)12
#define RENDER_WIDTH_PIXELS RENDER_WIDTH_TILES * TILE_SIZE_PIXELS
#define RENDER_HEIGHT_PIXELS RENDER_HEIGHT_TILES * TILE_SIZE_PIXELS

Texture GameSprites;

int load()
{
    GameSprites = LoadTexture("sprites/spritesheet.png");
}

int main()
{
    Tile map[256 * 6];

    const short windowWidth = RENDER_WIDTH_PIXELS * 4;
    const short windowHeight = RENDER_HEIGHT_PIXELS * 4;
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "Journey-Bound window testing");

    RenderTexture2D renderTexture = LoadRenderTexture(RENDER_WIDTH_PIXELS, RENDER_HEIGHT_PIXELS);
    Texture2D tileTexture = LoadTexture("sprites/texture.png");

    short renderScale = 1;

    while (!WindowShouldClose())
    {
        if (IsWindowResized())
        {
            
        }
        
        if (IsKeyPressed(KEY_RIGHT)) renderScale++;
        if (IsKeyPressed(KEY_LEFT)) renderScale--;
        
        BeginTextureMode(renderTexture);
        ClearBackground(LIGHTGRAY);
        
        for (short y = 0; y < RENDER_HEIGHT_TILES; y++)
        {
            for (short x = 0; x < RENDER_WIDTH_TILES; x++)
            {
                if ((x + y) % 2 == 0)
                {
                    DrawTexture(tileTexture, x * TILE_SIZE_PIXELS, y * TILE_SIZE_PIXELS, WHITE);
                }
            }
        }
        
        EndTextureMode();
        
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTextureEx(renderTexture.texture, Vector2Zero(), 0, renderScale, WHITE);
        
        DrawText(TextFormat("FPS: %i", GetFPS()), 5, 5, 15, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}