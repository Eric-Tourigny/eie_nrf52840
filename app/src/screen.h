#pragma once

#include "game_object.h"
#include "screen_info.h"

// The hex color for the screen's background
#define SCREEN_BACKGROUND_COLOR 0x3c88cf    

// Pixel width and height of screen
#define SCREEN_PX_WIDTH 320
#define SCREEN_PX_HEIGHT 240

// Subpixel width and height of screen
#define SCREEN_SUBPIXEL_WIDTH (SCREEN_PX_WIDTH << SUBPIXEL_SHIFT)
#define SCREEN_SUBPIXEL_HEIGHT (SCREEN_PX_HEIGHT << SUBPIXEL_SHIFT)

// Initial screen to display
#define STARTING_SCREEN_ROW 3
#define STARTING_SCREEN_COL 1

// Pixel width and height for tiles composing world
#define TILE_PX_WIDTH 15
#define TILE_PX_HEIGHT 15

// Subpixel width and height for tiles composing world
#define TILE_SUBPIXEL_WIDTH (TILE_PX_WIDTH << SUBPIXEL_SHIFT)
#define TILE_SUBPIXEL_HEIGHT (TILE_PX_HEIGHT << SUBPIXEL_SHIFT)

// Amount of pixels to hide past the edge of the screen
#define SCREEN_PX_LEFT_HIDDEN 13
#define SCREEN_PX_RIGHT_HIDDEN 12
#define SCREEN_PX_BOTTOM_HIDDEN 7
#define SCREEN_PX_TOP_HIDDEN 8

// Margin of extra subpixels to for tiles on edge of screen
#define SCREEN_SUBPIXEL_LEFT_EXTRA (TILE_SUBPIXEL_WIDTH - (SCREEN_PX_LEFT_HIDDEN << SUBPIXEL_SHIFT))
#define SCREEN_SUBPIXEL_RIGHT_EXTRA (TILE_SUBPIXEL_WIDTH - (SCREEN_PX_RIGHT_HIDDEN << SUBPIXEL_SHIFT))
#define SCREEN_SUBPIXEL_BOTTOM_EXTRA (TILE_SUBPIXEL_WIDTH - (SCREEN_PX_BOTTOM_HIDDEN << SUBPIXEL_SHIFT))
#define SCREEN_SUBPIXEL_TOP_EXTRA (TILE_SUBPIXEL_WIDTH - (SCREEN_PX_TOP_HIDDEN << SUBPIXEL_SHIFT))

// The maximum number of game objects appearing on a screen
#define MAX_NUM_GAME_OBJECTS 200            

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
screen_t* get_active_screen();
