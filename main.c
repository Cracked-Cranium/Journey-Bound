#include "stdio.h"

#include "raylib.h"
#include "raymath.h"

#include "enums.c"
#include "structs.c"
#include "item_functions.c"

#define PIXELS_PER_UNIT (short)4

#define TILE_SIZE_UNITS (short)16

#define TILE_SIZE_PIXELS (TILE_SIZE_UNITS * PIXELS_PER_UNIT)

#define RENDER_WIDTH_TILES (short)18
#define RENDER_HEIGHT_TILES (short)12

#define RENDER_WIDTH_UNITS (RENDER_WIDTH_TILES * TILE_SIZE_UNITS)
#define RENDER_HEIGHT_UNITS (RENDER_HEIGHT_TILES * TILE_SIZE_UNITS)

#define RENDER_WIDTH_PIXELS (RENDER_WIDTH_UNITS * PIXELS_PER_UNIT)
#define RENDER_HEIGHT_PIXELS (RENDER_HEIGHT_UNITS * PIXELS_PER_UNIT)

float MaxRenderScale(short windowWidth, short windowHeight)
{
    // targetSize = renderSize * renderScale
    // renderScale = targetSize / renderSize

    float horizontalScale = windowWidth / (float)RENDER_WIDTH_PIXELS;
    float verticalScale = windowHeight / (float)RENDER_HEIGHT_PIXELS;

    return fminf(horizontalScale, verticalScale);
}

int main()
{
    Tile map[256 * 6];
    short windowWidth = RENDER_WIDTH_PIXELS;
    short windowHeight = RENDER_HEIGHT_PIXELS;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Make the window resizeble
    InitWindow(windowWidth, windowHeight, "Journey-Bound window testing");

    RenderTexture2D renderTexture = LoadRenderTexture(RENDER_WIDTH_PIXELS, RENDER_HEIGHT_PIXELS); // The game will be rendered to a texture before being scaled to fit the players window
    float renderScale = 0.75;

    Texture2D tileTexture = LoadTexture("sprites/texture.png");
    Texture2D playerTexture = LoadTexture("sprites/player.png");

    Camera2D camera = (Camera2D){
        {RENDER_WIDTH_PIXELS * 0.5, RENDER_HEIGHT_PIXELS * 0.5},
        {0, 0},
        0,
        1};

    short playerPixelX = 0;
    short playerPixelY = 0;
    Vector2 playerPos = {0, 0};
    short playerSpeed = 256;

    while (!WindowShouldClose())
    {
        windowWidth = GetScreenWidth();
        windowHeight = GetScreenHeight();

        if (IsWindowResized())
        {
            renderScale = MaxRenderScale(windowWidth, windowHeight);
        }

        Vector2 playerMovement = {0, 0};

        if (IsKeyDown(KEY_RIGHT))
            playerMovement.x++;
        if (IsKeyDown(KEY_LEFT))
            playerMovement.x--;
        if (IsKeyDown(KEY_DOWN))
            playerMovement.y++;
        if (IsKeyDown(KEY_UP))
            playerMovement.y--;

        if (playerMovement.x != 0 || playerMovement.y != 0)
        {
            Vector2 normailised = Vector2Normalize(playerMovement);
            Vector2 scaled = Vector2Scale(normailised, GetFrameTime() * playerSpeed);
            playerPos = Vector2Add(playerPos, scaled);
        }
        else
        {
            playerPos.x = roundf(playerPos.x);
            playerPos.y = roundf(playerPos.y);
        }

        playerPixelX = playerPos.x;
        playerPixelY = playerPos.y;

        camera.target.x = playerPixelX;
        camera.target.y = playerPixelY;

        /*
        ===How drawing works!===
        BeginDrawing();

            BeginTextureMode(renderTexture);

                BeginMode2D(camera);

                    Here you draw the game content (the world and entities) which will then automatically be offset by the camera position

                EndDrawing();

                The camera automatically takes what it sees and draws it to the current context (in this case, the renderTexture)
                Here you can also draw the inventory or other hud stuff that you want to be pixel perfect

            EndTextureMode();

            Here you draw the renderTexture to the screen at an appropriate position and scale
            You can also draw debug stuff that will not be pixel perfect

        EndMode2D();
        */

        BeginDrawing();
        BeginTextureMode(renderTexture);
        BeginMode2D(camera);

        ClearBackground(LIGHTGRAY);

        // Draw tiles
        for (short y = 0; y < RENDER_HEIGHT_TILES; y++)
        {
            for (short x = 0; x < RENDER_WIDTH_TILES; x++)
            {
                if ((x + y) % 2 == 0)
                {
                    DrawTextureEx(
                        tileTexture,
                        (Vector2){x * TILE_SIZE_PIXELS, y * TILE_SIZE_PIXELS},
                        0,
                        PIXELS_PER_UNIT,
                        WHITE);
                }
            }
        }

        DrawTextureEx(
            playerTexture,
            (Vector2){playerPixelX, playerPixelY},
            0,
            PIXELS_PER_UNIT,
            WHITE);

        EndMode2D(); // Draw pixel perfect, but not in camera view

        EndTextureMode();

        // Draw render and debug
        ClearBackground(BLACK);

        short renderTargetWidth = RENDER_WIDTH_PIXELS * renderScale;
        short renderTargetHeight = RENDER_HEIGHT_PIXELS * renderScale;
        DrawTexturePro(
            renderTexture.texture,
            (Rectangle){
                0,
                0,
                renderTexture.texture.width,
                -renderTexture.texture.height}, // Negative, because textures are upside-down
            (Rectangle){
                (windowWidth - renderTargetWidth) * 0.5,
                (windowHeight - renderTargetHeight) * 0.5,
                renderTargetWidth,
                renderTargetHeight},
            Vector2Zero(),
            0,
            WHITE);

        // Debug drawing
        /* for (short x = 0; x < windowWidth; x += RENDER_WIDTH_PIXELS)
        { DrawLine(x, 0, x, windowHeight, RED); }
        for (short y = 0; y < windowHeight; y += RENDER_HEIGHT_PIXELS)
        { DrawLine(0, y, windowWidth, y, RED); } */

        DrawText(TextFormat("FPS: %i", GetFPS()), 5, 5, 15, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}