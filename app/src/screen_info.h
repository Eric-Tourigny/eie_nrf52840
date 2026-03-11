#pragma once

#include <lvgl.h>
#include "game_object.h"

#ifdef __INTELLISENSE__
  #include <modules/lib/gui/lvgl/lvgl.h>
#endif

#define NUM_SCREEN_ROWS 4                   // The number of rows in the grid of screens
#define NUM_SCREEN_COLS 3                   // The number of columns in the grid of screens


// The information necessary to create a screen - a set of tiles displayed to the user
struct __screen_t {
  game_object_t* objects;
  uint32_t num_objects;
} typedef screen_t;