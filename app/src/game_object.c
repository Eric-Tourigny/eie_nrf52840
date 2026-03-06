#include "game_object.h"
#include <zephyr/kernel.h>


void init_game_object(game_object_t* obj, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const lv_image_dsc_t* image_src, lv_obj_t* parent) {
  // Set the internal position and size of the game object
  obj->pos.x = x << SUBPIXEL_SHIFT;
  obj->pos.y = y << SUBPIXEL_SHIFT;
  obj->w = w << SUBPIXEL_SHIFT;
  obj->h = h << SUBPIXEL_SHIFT;

  // Create the image representing the game object
  lv_obj_t* image = lv_image_create(parent);
  lv_image_set_src(image, image_src);
  lv_obj_set_pos(image, x, y);
  obj->sprite = image;
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
  lv_obj_delete_async(obj->sprite);

  // Free the memory allocated for the game object
  k_free(obj);
}
