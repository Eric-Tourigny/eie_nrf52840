#pragma once

#include <lvgl.h>

#ifdef __INTELLISENSE__
  #include <modules/lib/gui/lvgl/lvgl.h>
#endif

#define NUM_SCREEN_ROWS 4                   // The number of rows in the grid of screens
#define NUM_SCREEN_COLS 3                   // The number of columns in the grid of screens

// The information necessary to create the rectangle tiles making up the terrain
struct ___tile_info_t {
  const lv_image_dsc_t* sprite;
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
} typedef tile_info_t;

// The information necessary to create a screen - a set of tiles displayed to the user
struct __screen_t {
  const tile_info_t* objects;
  uint32_t num_objects;
} typedef screen_t;