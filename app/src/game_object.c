#include "game_object.h"
#include <zephyr/kernel.h>


void activate_game_object(game_object_t* obj, lv_obj_t* screen) {
  // Clear the old image, if the object is already active
  deactivate_game_object(obj);

  // Create the image representing the game object
  lv_obj_t* image = lv_image_create(screen);
  lv_obj_set_pos(image, obj->pos.x >> SUBPIXEL_SHIFT, obj->pos.y >> SUBPIXEL_SHIFT);
  lv_obj_set_size(image, obj->w >> SUBPIXEL_SHIFT, obj->h >> SUBPIXEL_SHIFT);
  lv_image_set_src(image, obj->sprite);
  lv_image_set_inner_align(image, LV_IMAGE_ALIGN_TILE);   // Enable tile mosaic
  obj->image = image;
}

void deactivate_game_object(game_object_t* obj) {
  // Clear the LVGL resources for the game object's sprite
  if (obj->image != NULL) {
    lv_obj_delete(obj->image);
    obj->image = NULL;
  }
}

void set_game_object_sprite(game_object_t* obj, const lv_image_dsc_t* sprite) {
  if (sprite != obj->sprite) {
    obj->sprite = sprite;
    if (obj->image != NULL) {
      lv_image_set_src(obj->image, sprite);
    }
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
