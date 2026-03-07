#pragma once

#include <lvgl.h>
#include "game_object.h"

#ifdef __INTELLISENSE__
  #include <modules/lib/gui/lvgl/lvgl.h>
#endif

#define SCREEN_BACKGROUND_COLOR 0x3c88cf    // The hex color to set the background canvas to
#define TILE_WIDTH 15                       // The width in pixels of each tile
#define TILE_HEIGHT 15                      // The height in pixels of each tile
#define MAX_NUM_GAME_OBJECTS 200            // The maximum number of game objects appearing on a screen

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
 * Set `screen` as the active screen, destroying the objects on the old screen and placing the objects for the new screen onto `parent`.
 */
game_object_list_t* activate_screen(screen_t* screen, lv_obj_t* parent);
