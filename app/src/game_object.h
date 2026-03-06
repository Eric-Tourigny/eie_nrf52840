#pragma once

#include <inttypes.h>
#include <lvgl.h>

#ifdef __INTELLISENSE__
  #include <modules/lib/gui/lvgl/lvgl.h>
#endif

// The first 8 bits of each position value represents a subpixel value, allowing for clean movement
// Values should be shifted by SUBPIXEL shift before being passed into LVGL
#define SUBPIXEL_SHIFT 8

// Represents a vector with components on the x and y axes
struct __vector2_t {
  int32_t x;
  int32_t y;
} typedef vector2_t;

// Represents an object within the game, which is displayed on the screen with the LVGL image `sprite` at (`pos.x`, `pos.y`)
struct __game_object_t {
  lv_obj_t* sprite;
  vector2_t pos;
  int32_t w;
  int32_t h;
} typedef game_object_t;

/*
 * Initialize an new game object at the memory position `obj`. 
 * Places the sprite `image_src` at (`x`, `y`) (px) relative to `parent`.
 * The objects hitbox has width `w` and height `h` (px).
 */
void init_game_object(game_object_t* obj, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const lv_image_dsc_t* image_src, lv_obj_t* parent);

/*
 * Allocate, initialize and return a new game object.
 * Places the sprite `image_src` at (`x`, `y`) (px) relative to `parent`.
 * The objects hitbox has width `w` and height `h` (px).
 */
game_object_t* create_game_object(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const lv_image_dsc_t* image_src, lv_obj_t* parent);

/*
 * Clear the resources for and deallocate the memory for the game object `obj`, allocated through `create_game_object`
 */
void destroy_game_object(game_object_t* obj);
