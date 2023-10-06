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

#define MAP_WIDTH_TILES (short)32
#define MAP_HEIGHT_TILES (short)32

float GetMaxRenderScale(short window_width, short window_height)
{
    // targetSize = renderSize * renderScale
    // renderScale = targetSize / renderSize

    float horizontal_scale = window_width / (float)RENDER_WIDTH_PIXELS;
    float vertical_scale = window_height / (float)RENDER_HEIGHT_PIXELS;

    return fminf(horizontal_scale, vertical_scale);
}

void DrawGroundTiles(short *game_map, Texture2D *sprite_sheet_ptr)
{
    for (short y = 0; y < MAP_HEIGHT_TILES; y++)
    {
        for (short x = 0; x < MAP_WIDTH_TILES; x++)
        {
            short index = game_map[x + y * MAP_WIDTH_TILES];

            DrawTexturePro(
                *sprite_sheet_ptr,
                (Rectangle){
                    (index % TILE_SIZE_UNITS) * TILE_SIZE_UNITS,
                    (index / TILE_SIZE_UNITS) * TILE_SIZE_UNITS,
                    TILE_SIZE_UNITS,
                    TILE_SIZE_UNITS},
                (Rectangle){
                    x * TILE_SIZE_PIXELS,
                    y * TILE_SIZE_PIXELS,
                    TILE_SIZE_PIXELS,
                    TILE_SIZE_PIXELS},
                Vector2Zero(),
                0,
                WHITE);
        }
    }
}

int main()
{
    short window_width = RENDER_WIDTH_PIXELS;
    short window_height = RENDER_HEIGHT_PIXELS;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Make the window resizeble

    InitWindow(window_width, window_height, "Journey-Bound window testing");

    RenderTexture2D game_render_texture = LoadRenderTexture(RENDER_WIDTH_PIXELS, RENDER_HEIGHT_PIXELS); // The game will be rendered to a texture before being scaled to fit the players window
    float render_scale = 0.75;

    Texture2D tile_texture = LoadTexture("sprites/texture.png");
    Texture2D player_texture = LoadTexture("sprites/player.png");
    Texture2D ground_tiles_sprite_sheet = LoadTexture("sprites/BlockSprites.png");

    Camera2D game_camera = (Camera2D){
        {RENDER_WIDTH_PIXELS * 0.5, RENDER_HEIGHT_PIXELS * 0.5},
        {0, 0},
        0,
        1};

    short player_pixel_x = 0;
    short player_pixel_y = 0;
    Vector2 player_position = {0, 0};
    short player_speed = 256;

    //Preliminary map array
    short game_map[MAP_WIDTH_TILES * MAP_HEIGHT_TILES];

    for (short y = 0; y < MAP_HEIGHT_TILES; y++)
    {
        for (short x = 0; x < MAP_WIDTH_TILES; x++)
        {
            game_map[x + y * MAP_WIDTH_TILES] = (short)0;
            
            if (x % 6 == 0 && y % 8 == 0)
            {
                game_map[x + y * MAP_WIDTH_TILES] = (short)2;
            }
        }
    }

    while (!WindowShouldClose())
    {
        window_width = GetScreenWidth();
        window_height = GetScreenHeight();

        if (IsWindowResized())
        {
            render_scale = GetMaxRenderScale(window_width, window_height);
        }

        Vector2 player_movement = {0, 0};

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_RIGHT))
            player_movement.x++;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_LEFT))
            player_movement.x--;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
            player_movement.y++;
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
            player_movement.y--;

        if (player_movement.x != 0 || player_movement.y != 0)
        {
            Vector2 normalised = Vector2Normalize(player_movement);
            Vector2 scaled = Vector2Scale(normalised, GetFrameTime() * player_speed);
            player_position = Vector2Add(player_position, scaled);
        }
        else
        {
            player_position.x = roundf(player_position.x);
            player_position.y = roundf(player_position.y);
        }

        player_pixel_x = player_position.x;
        player_pixel_y = player_position.y;

        game_camera.target.x = player_pixel_x;
        game_camera.target.y = player_pixel_y;

        /*
        ===How drawing works!===
        BeginDrawing();

            BeginTextureMode(game_render_texture);

                BeginMode2D(game_camera);

                    Here you draw the game content (the world and entities) which will then automatically be offset by the game_camera position

                EndDrawing();

                The game_camera automatically takes what it sees and draws it to the current context (in this case, the game_render_texture)
                Here you can also draw the inventory or other hud stuff that you want to be pixel perfect

            EndTextureMode();

            Here you draw the game_render_texture to the screen at an appropriate position and scale
            You can also draw debug stuff that will not be pixel perfect

        EndMode2D();
        */

        BeginDrawing();
        ClearBackground((Color){32, 32, 32});

        BeginTextureMode(game_render_texture);
        ClearBackground(BLACK);

        BeginMode2D(game_camera);

        // Draw tiles
        DrawGroundTiles(game_map, &ground_tiles_sprite_sheet);

        DrawTextureEx(
            player_texture,
            (Vector2){player_pixel_x, player_pixel_y},
            0,
            PIXELS_PER_UNIT,
            WHITE);

        EndMode2D(); // Draw pixel perfect, but not in camera view

        EndTextureMode();

        // Draw render and debug
        short render_target_width = RENDER_WIDTH_PIXELS * render_scale;
        short render_target_height = RENDER_HEIGHT_PIXELS * render_scale;
        DrawTexturePro(
            game_render_texture.texture,
            (Rectangle){
                0,
                0,
                game_render_texture.texture.width,
                -game_render_texture.texture.height}, // Negative, because textures are upside-down
            (Rectangle){
                (window_width - render_target_width) * 0.5,
                (window_height - render_target_height) * 0.5,
                render_target_width,
                render_target_height},
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