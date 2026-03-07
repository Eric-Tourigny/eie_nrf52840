#pragma once

#include <lvgl.h>
#include "game_object.h"

#ifdef __INTELLISENSE__
  #include <modules/lib/gui/lvgl/lvgl.h>
#endif

#define TILE_WIDTH 15                   // The width in pixels of each tile
#define TILE_HEIGHT 15                  // The height in pixels of each tile
#define MAX_NUM_GAME_OBJECTS 100        // The maximum number of game objects appearing on a screen

// The information necessary to create the rectangle tiles making up the terrain
struct ___tile_info_t {
    const lv_image_dsc_t* sprite;
    uint32_t x;
    uint32_t y;    
} typedef tile_info_t;

// The information necessary to create a screen - a set of tiles displayed to the user
struct __screen_t {
    tile_info_t* objects;
    uint32_t num_objects;
} typedef screen_t;

extern screen_t SCREEN1;

/*
 * Set `screen` as the active screen, destroying the objects on the old screen and placing the objects for the new screen onto `parent`.
 */
game_object_t** activate_screen(screen_t* screen, lv_obj_t* parent);
