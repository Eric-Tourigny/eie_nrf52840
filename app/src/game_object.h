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
  lv_obj_t* image;
  const lv_image_dsc_t* sprite;
  vector2_t pos;
  int32_t w;
  int32_t h;
} typedef game_object_t;

/*
 * Display the game object on `screen`
 */
void activate_game_object(game_object_t* obj, lv_obj_t* screen);

/*
 * Stop displaying the game object and free lvgl resources
 */
void deactivate_game_object(game_object_t* obj);

/*
 * Update `obj`'s image to display `sprite`, if it different then it's current sprite
 */
void set_game_object_sprite(game_object_t* obj, const lv_image_dsc_t* sprite);

/*
 * Returns whether the hitboxes of `obj1` and `obj2` are currently overlapping
 */
bool check_collision(game_object_t* obj1, game_object_t* obj2);
