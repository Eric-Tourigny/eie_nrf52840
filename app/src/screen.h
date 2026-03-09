#pragma once

#include <lvgl.h>
#include "game_object.h"

#ifdef __INTELLISENSE__
  #include <modules/lib/gui/lvgl/lvgl.h>
#endif

#define SCREEN_BACKGROUND_COLOR 0x3c88cf    // The hex color to set the background canvas to
#define SCREEN_WIDTH (320 << SUBPIXEL_SHIFT)
#define SCREEN_HEIGHT (240 << SUBPIXEL_SHIFT)

#define STARTING_SCREEN_ROW 3
#define STARTING_SCREEN_COL 1

#define TILE_WIDTH 15                       // The width in pixels of each tile
#define TILE_HEIGHT 15                      // The height in pixels of each tile
#define MAX_NUM_GAME_OBJECTS 200            // The maximum number of game objects appearing on a screen
#define NUM_SCREEN_ROWS 4                   // The number of rows in the grid of screens
#define NUM_SCREEN_COLS 3                   // The number of columns in the grid of screens

// The information necessary to create the rectangle tiles making up the terrain
struct ___tile_info_t {
  const lv_image_dsc_t* sprite;
  uint32_t x;
  uint32_t y;    
} typedef tile_info_t;

// The information necessary to create a screen - a set of tiles displayed to the user
struct __screen_t {
  const tile_info_t* objects;
  uint32_t num_objects;
} typedef screen_t;

extern screen_t SCREEN1;

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
