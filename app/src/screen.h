#pragma once

#include "game_object.h"


#define SCREEN_BACKGROUND_COLOR 0x3c88cf    // The hex color to set the background canvas to
#define SCREEN_WIDTH (320 << SUBPIXEL_SHIFT)
#define SCREEN_HEIGHT (240 << SUBPIXEL_SHIFT)

#define STARTING_SCREEN_ROW 3
#define STARTING_SCREEN_COL 1

#define TILE_WIDTH 15                       // The width in pixels of each tile
#define TILE_HEIGHT 15                      // The height in pixels of each tile
#define MAX_NUM_GAME_OBJECTS 200            // The maximum number of game objects appearing on a screen

// An array of game objects, with associated length
struct __game_object_list_t {
  uint32_t len;
  game_object_t* game_objects;
} typedef game_object_list_t;

/*
 * Initialize and activate the game screen
 */
void init_screen();

/*
 * Set screen (`row`, `col`) as the active screen, destroying the objects on the old screen
 */
void activate_screen(int32_t row, int32_t col);

/*
 * Shift the active screen by `row_delta`, `col_delta`, destroying the objects on the old screen
 */
void shift_screen(int32_t row_delta, int32_t col_delta);

/*
 * Get all the game objects on the currently active screen
 */
game_object_list_t* get_active_screen_game_objects();
