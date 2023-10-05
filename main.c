#include "raylib.h"
#include "raymath.h"

#include "enums.c"
#include "structs.c"

#define TILE_SIZE_PIXELS (short)16
#define RENDER_WIDTH_TILES (short)18
#define RENDER_HEIGHT_TILES (short)12
#define RENDER_WIDTH_PIXELS RENDER_WIDTH_TILES * TILE_SIZE_PIXELS
#define RENDER_HEIGHT_PIXELS RENDER_HEIGHT_TILES * TILE_SIZE_PIXELS

//Returns the largest scale the render could be without overflowing the window
short UpdateRenderScale(short windowWidth, short windowHeight)
{
    short renderScale = 1;
    
    while (
        RENDER_WIDTH_PIXELS * (renderScale + 1) <= windowWidth &&
        RENDER_HEIGHT_PIXELS * (renderScale + 1) <= windowHeight
    )
    {
        renderScale++;
    }

    return renderScale;
}

int main()
{
    Tile map[256 * 6];
    
    short windowWidth = RENDER_WIDTH_PIXELS * 4.5;
    short windowHeight = RENDER_HEIGHT_PIXELS * 4.5;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE); //Make the window resizeble
    InitWindow(windowWidth, windowHeight, "Journey-Bound window testing");

    RenderTexture2D renderTexture = LoadRenderTexture(RENDER_WIDTH_PIXELS, RENDER_HEIGHT_PIXELS); //The game will be rendered to a texture before being scaled to fit the players window
    short renderScale = UpdateRenderScale(windowWidth, windowHeight);

    Texture2D GameSprites = LoadTexture("sprites/spritesheet.png");
    Texture2D tileTexture = LoadTexture("sprites/texture.png");

    while (!WindowShouldClose())
    {
        windowWidth = GetScreenWidth();
        windowHeight = GetScreenHeight();
        
        if (IsWindowResized()) renderScale = UpdateRenderScale(windowWidth, windowHeight);
        
        //===Draw game content===
        BeginTextureMode(renderTexture);
        ClearBackground(LIGHTGRAY);
        
        //Draw tiles
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
        
        //===Draw render and debug===
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTextureEx(renderTexture.texture, Vector2Zero(), 0, renderScale, WHITE);

        //Debug drawing
        for (short x = 0; x < windowWidth; x += RENDER_WIDTH_PIXELS)
        { DrawLine(x, 0, x, windowHeight, RED); }
        for (short y = 0; y < windowHeight; y += RENDER_HEIGHT_PIXELS)
        { DrawLine(0, y, windowWidth, y, RED); }
        
        DrawText(TextFormat("FPS: %i", GetFPS()), 5, 5, 15, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}