#include "stdio.h"
#include "stdlib.h"

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
#define CHUNK_COUNT (short)9

// #define PLAYER_SPRINT_SPEED (float)2

#include "enums.c"
#include "structs.c"
#include "item_functions.c"

// Returns the maximum render scale that still fits in the window
float GetMaxRenderScale(short window_width, short window_height)
{
    // targetSize = renderSize * renderScale <=> renderScale = targetSize / renderSize

    float horizontal_scale = window_width / (float)RENDER_WIDTH_PIXELS;
    float vertical_scale = window_height / (float)RENDER_HEIGHT_PIXELS;

    return fminf(horizontal_scale, vertical_scale);
}

// Draws a ground tile texture from the corresponding index
void DrawTileFromIndex(short texture_index, Short2 pos_tiles, Texture2D *sprite_sheet_ptr)
{
    DrawTexturePro(
        *sprite_sheet_ptr,
        (Rectangle){
            (texture_index % TILE_SIZE_UNITS) * TILE_SIZE_UNITS,
            (texture_index / TILE_SIZE_UNITS) * TILE_SIZE_UNITS,
            TILE_SIZE_UNITS,
            TILE_SIZE_UNITS},
        (Rectangle){
            pos_tiles.x * TILE_SIZE_PIXELS,
            pos_tiles.y * TILE_SIZE_PIXELS,
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

            DrawTileFromIndex(index, (Short2){x, y}, sprite_sheet_ptr);
        }
    }
}

// Loads tiles from the game map to a chunk
Chunk LoadChunk(short game_map[], Short2 target_chunk_pos)
{
    Chunk chunk;
    chunk.chunk_pos = target_chunk_pos;

    short chunk_index = target_chunk_pos.x * CHUNK_SIZE_TILES + target_chunk_pos.y * CHUNK_SIZE_TILES * MAP_WIDTH_TILES;

    for (short y = 0; y < CHUNK_SIZE_TILES; y++)
    {
        for (short x = 0; x < CHUNK_SIZE_TILES; x++)
        {
            chunk.tiles[x + y * CHUNK_SIZE_TILES] = game_map[chunk_index + x + y * MAP_WIDTH_TILES];
        }
    }

    return chunk;
}

// Draws the contents of all chunks in a chunk array
void DrawChunks(DynChunkArr arr, Texture2D *sprite_sheet_ptr)
{
    for (short n = 0; n < arr.count; n++)
    {
        for (short y = 0; y < CHUNK_SIZE_TILES; y++)
        {
            for (short x = 0; x < CHUNK_SIZE_TILES; x++)
            {
                short texture_index = arr.chunks[n].tiles[x + y * CHUNK_SIZE_TILES];

                DrawTileFromIndex(
                    texture_index,
                    Short2Add(Short2Scale(arr.chunks[n].chunk_pos, CHUNK_SIZE_TILES), (Short2){x, y}),
                    sprite_sheet_ptr);
            }
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
    short player_sprint_speed = 1;

    // Preliminary map array
    short game_map[MAP_WIDTH_TILES * MAP_HEIGHT_TILES];

    for (short y = 0; y < MAP_HEIGHT_TILES; y++)
    {
        for (short x = 0; x < MAP_WIDTH_TILES; x++)
        {
            game_map[x + y * MAP_WIDTH_TILES] = (short)0;

            // Make pattern
            if (x % 5 == 0 && y % 6 == 0)
                game_map[x + y * MAP_WIDTH_TILES] = (short)16;

            if (x % 4 == 0 && y % 5 == 0)
                game_map[x + y * MAP_WIDTH_TILES] = (short)3;
        }
    }

    // Prelimenary chunk system
    Short2 player_pos_chunk;
    Short2 player_pos_chunk_prev;

    DynChunkArr game_generated_chunks;
    InitDCA(&game_generated_chunks, 2);
    AddDCA(&game_generated_chunks, LoadChunk(game_map, (Short2){0, 0}));

    while (!WindowShouldClose())
    {
        Vector2 player_movement = {0, 0};

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
            player_movement.x++;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
            player_movement.x--;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
            player_movement.y++;
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
            player_movement.y--;

        if (IsKeyDown(KEY_LEFT_SHIFT))
            player_sprint_speed = 2;
        else
            player_sprint_speed = 1;

        if (player_movement.x != 0 || player_movement.y != 0)
        {
            Vector2 normalised = Vector2Normalize(player_movement);
            Vector2 scaled = Vector2Scale(normalised, GetFrameTime() * player_speed * player_sprint_speed);
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

        // Whack formula because floor can handle negatives as opposed to truncating
        player_pos_chunk = (Short2){
            (short)floorf(((float)player_pixel_x / TILE_SIZE_PIXELS) / CHUNK_SIZE_TILES),
            (short)floorf(((float)player_pixel_y / TILE_SIZE_PIXELS) / CHUNK_SIZE_TILES)};

        // When entering a different chunk
        if (player_pos_chunk.x != player_pos_chunk_prev.x || player_pos_chunk.y != player_pos_chunk_prev.y)
        {
            bool already_exists = false;
            
            for (short i = 0; i < game_generated_chunks.count; i++)
            {
                if (
                    game_generated_chunks.chunks[i].chunk_pos.x == player_pos_chunk.x &&
                    game_generated_chunks.chunks[i].chunk_pos.y == player_pos_chunk.y)
                {
                    already_exists = true;
                    break;
                }
            }

            // Only adds new chunk if it's not already loaded
            if (!already_exists) AddDCA(&game_generated_chunks, LoadChunk(game_map, player_pos_chunk));
        }

        player_pos_chunk_prev = player_pos_chunk;

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
        // DrawGroundTiles(game_map, &ground_tiles_sprite_sheet);
        DrawChunks(game_generated_chunks, &ground_tiles_sprite_sheet);

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