#include "game_object.h"
#include <zephyr/kernel.h>


void init_game_object(game_object_t* obj, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const lv_image_dsc_t* sprite, lv_obj_t* parent) {
  // Set the internal position and size of the game object
  obj->pos.x = x << SUBPIXEL_SHIFT;
  obj->pos.y = y << SUBPIXEL_SHIFT;
  obj->w = w << SUBPIXEL_SHIFT;
  obj->h = h << SUBPIXEL_SHIFT;

  // Create the image representing the game object
  lv_obj_t* image = lv_image_create(parent);
  lv_obj_set_pos(image, x, y);
  lv_obj_set_size(image, w, h);
  lv_image_set_src(image, sprite);
  lv_image_set_inner_align(image, LV_IMAGE_ALIGN_TILE); 
  obj->image = image;
  obj->sprite = sprite; 
}

game_object_t* create_game_object(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const lv_image_dsc_t* image_src, lv_obj_t* parent) {
  // Allocate memory for the game object
  game_object_t* obj = k_malloc(sizeof(game_object_t));

  // Initialize the game object
  init_game_object(obj, x, y, w, h, image_src, parent);

  return obj;
}

void destroy_game_object(game_object_t* obj) {
  // Clear the LVGL resources for the game object's sprite
  lv_obj_delete_async(obj->image);

  // Free the memory allocated for the game object
  k_free(obj);
}

void set_game_object_sprite(game_object_t* obj, const lv_image_dsc_t* sprite) {
  if (sprite != obj->sprite) {
    obj->sprite = sprite;
    lv_image_set_src(obj->image, sprite);
  }
}

bool check_collision(game_object_t* obj1, game_object_t* obj2) {
  // Check if the hitbox's horizontal ranges overlap
  bool is_right_of_left        = obj1->pos.x + obj1->w > obj2->pos.x;
  bool is_left_of_right        = obj1->pos.x < obj2->pos.x + obj2->w;
  bool within_horizontal_range = is_right_of_left && is_left_of_right;

  // Check if the hitbox's vertical ranges overlap
  bool is_below_top            = obj1->pos.y + obj1->h > obj2->pos.y;
  bool is_above_bottom         = obj1->pos.y < obj2->pos.y + obj2->h;
  bool within_vertical_range   = is_below_top && is_above_bottom;

  return within_horizontal_range && within_vertical_range;
}
