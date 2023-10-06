#include "stdio.h"

#include "raylib.h"
#include "raymath.h"

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

#define CHUNK_SIZE_TILES (short)8

#include "enums.c"
#include "structs.c"
#include "item_functions.c"

float GetMaxRenderScale(short window_width, short window_height)
{
    // targetSize = renderSize * renderScale
    // renderScale = targetSize / renderSize

    float horizontal_scale = window_width / (float)RENDER_WIDTH_PIXELS;
    float vertical_scale = window_height / (float)RENDER_HEIGHT_PIXELS;

    return fminf(horizontal_scale, vertical_scale);
}

void DrawTileFromIndex(short texture_index, short pos_x_tiles, short pos_y_tiles, Texture2D *sprite_sheet_ptr)
{
    DrawTexturePro(
        *sprite_sheet_ptr,
        (Rectangle){
            (texture_index % TILE_SIZE_UNITS) * TILE_SIZE_UNITS,
            (texture_index / TILE_SIZE_UNITS) * TILE_SIZE_UNITS,
            TILE_SIZE_UNITS,
            TILE_SIZE_UNITS},
        (Rectangle){
            pos_x_tiles * TILE_SIZE_PIXELS,
            pos_y_tiles * TILE_SIZE_PIXELS,
            TILE_SIZE_PIXELS,
            TILE_SIZE_PIXELS},
        Vector2Zero(),
        0,
        WHITE);
}

void DrawGroundTiles(short game_map[], Texture2D *sprite_sheet_ptr)
{
    for (short y = 0; y < MAP_HEIGHT_TILES; y++)
    {
        for (short x = 0; x < MAP_WIDTH_TILES; x++)
        {
            short index = game_map[x + y * MAP_WIDTH_TILES];

            DrawTileFromIndex(index, x, y, sprite_sheet_ptr);
        }
    }
}

void LoadChunk(short game_map[], short chunk_x, short chunk_y, Chunk *target)
{
    (*target).chunk_x = chunk_x;
    (*target).chunk_y = chunk_y;
    
    short chunk_index = chunk_x * CHUNK_SIZE_TILES + chunk_y * CHUNK_SIZE_TILES * MAP_WIDTH_TILES;

    for (short y = 0; y < CHUNK_SIZE_TILES; y++)
    {
        for (short x = 0; x < CHUNK_SIZE_TILES; x++)
        {
            (*target).tiles[x + y * CHUNK_SIZE_TILES] = game_map[chunk_index + x + y * MAP_WIDTH_TILES];
        }
    }
}

void DrawChunk(Chunk *chunk_ptr, Texture2D *sprite_sheet_ptr)
{
    for (short y = 0; y < CHUNK_SIZE_TILES; y++)
    {
        for (short x = 0; x < CHUNK_SIZE_TILES; x++)
        {
            short texture_index = (*chunk_ptr).tiles[x + y * CHUNK_SIZE_TILES];
            
            DrawTileFromIndex(
                texture_index,
                (*chunk_ptr).chunk_x * CHUNK_SIZE_TILES + x,
                (*chunk_ptr).chunk_y * CHUNK_SIZE_TILES + y,
                sprite_sheet_ptr);
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

    // Preliminary map array
    short game_map[MAP_WIDTH_TILES * MAP_HEIGHT_TILES];

    for (short y = 0; y < MAP_HEIGHT_TILES; y++)
    {
        for (short x = 0; x < MAP_WIDTH_TILES; x++)
        {
            game_map[x + y * MAP_WIDTH_TILES] = (short)0;

            if (x % 2 == 0)
            {
                game_map[x + y * MAP_WIDTH_TILES] = (short)2;
            }

            if (y % 3 == 0)
            {
                game_map[x + y * MAP_WIDTH_TILES] = (short)3;
            }
        }
    }

    // Prelimenary chunk system
    Chunk current_chunk;

    LoadChunk(game_map, 0, 0, &current_chunk);

    short player_pos_x_chunk;
    short player_pos_y_chunk;
    short player_pos_x_chunk_prev;
    short player_pos_y_chunk_prev;

    while (!WindowShouldClose())
    {
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

        game_camera.target.x = player_pixel_x + TILE_SIZE_PIXELS / 2;
        game_camera.target.y = player_pixel_y + TILE_SIZE_PIXELS / 2;

        player_pos_x_chunk = player_pixel_x / TILE_SIZE_PIXELS / CHUNK_SIZE_TILES;
        player_pos_y_chunk = player_pixel_y / TILE_SIZE_PIXELS / CHUNK_SIZE_TILES;

        if (player_pos_x_chunk != player_pos_x_chunk_prev || player_pos_y_chunk != player_pos_y_chunk_prev)
        {
            LoadChunk(game_map, player_pos_x_chunk, player_pos_y_chunk, &current_chunk);
        }

        player_pos_x_chunk_prev = player_pos_x_chunk;
        player_pos_y_chunk_prev = player_pos_y_chunk;
        

        window_width = GetScreenWidth();
        window_height = GetScreenHeight();

        if (IsWindowResized())
        {
            render_scale = GetMaxRenderScale(window_width, window_height);
        }

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
        ClearBackground(BLACK);

        BeginTextureMode(game_render_texture);
        ClearBackground((Color){32, 32, 32, 255});

        BeginMode2D(game_camera);

        // Draw tiles
        //DrawGroundTiles(game_map, &ground_tiles_sprite_sheet);
        DrawChunk(&current_chunk, &ground_tiles_sprite_sheet);

        // Draw player
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