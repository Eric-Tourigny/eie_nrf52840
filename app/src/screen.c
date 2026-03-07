#include "screen.h"
#include <zephyr/sys/util.h>

extern const lv_image_dsc_t SpriteRockTile;

static game_object_t game_objects[MAX_NUM_GAME_OBJECTS + 1];
static game_object_list_t game_object_list = {.game_objects = game_objects, .len = 0};

game_object_list_t* activate_screen(screen_t* screen, lv_obj_t* parent) {
  // Set parent's background color
  lv_obj_set_style_bg_color(parent, lv_color_hex(SCREEN_BACKGROUND_COLOR), LV_PART_MAIN);
  
  // Create the game objects for the new screen
  for (int i = 0; i < screen->num_objects; i++) {
    const tile_info_t* tile_info = &screen->objects[i];
    init_game_object(&game_object_list.game_objects[i], tile_info->x, tile_info->y, TILE_WIDTH, TILE_HEIGHT, tile_info->sprite, parent);
  }
  game_object_list.len = screen->num_objects;

  return &game_object_list;
}


/***************************************************************************************************************************************
 * Screen 1
 ***************************************************************************************************************************************/

static const tile_info_t screen1_tiles[] = {
  {&SpriteRockTile, 0, 0},
  {&SpriteRockTile, 0, 15},
  {&SpriteRockTile, 0, 30},
  {&SpriteRockTile, 0, 45},
  {&SpriteRockTile, 0, 60},
  {&SpriteRockTile, 0, 75},
  {&SpriteRockTile, 0, 90},
  {&SpriteRockTile, 0, 105},
  {&SpriteRockTile, 0, 120},
  {&SpriteRockTile, 0, 135},
  {&SpriteRockTile, 0, 150},
  {&SpriteRockTile, 0, 225},
  {&SpriteRockTile, 15, 0},
  {&SpriteRockTile, 15, 15},
  {&SpriteRockTile, 15, 30},
  {&SpriteRockTile, 15, 45},
  {&SpriteRockTile, 15, 60},
  {&SpriteRockTile, 15, 75},
  {&SpriteRockTile, 15, 90},
  {&SpriteRockTile, 15, 105},
  {&SpriteRockTile, 15, 120},
  {&SpriteRockTile, 15, 135},
  {&SpriteRockTile, 15, 225},
  {&SpriteRockTile, 30, 15},
  {&SpriteRockTile, 30, 30},
  {&SpriteRockTile, 30, 45},
  {&SpriteRockTile, 30, 60},
  {&SpriteRockTile, 30, 75},
  {&SpriteRockTile, 30, 90},
  {&SpriteRockTile, 30, 105},
  {&SpriteRockTile, 30, 120},
  {&SpriteRockTile, 30, 225},
  {&SpriteRockTile, 45, 45},
  {&SpriteRockTile, 45, 60},
  {&SpriteRockTile, 45, 75},
  {&SpriteRockTile, 45, 90},
  {&SpriteRockTile, 45, 105},
  {&SpriteRockTile, 45, 225},
  {&SpriteRockTile, 60, 45},
  {&SpriteRockTile, 60, 60},
  {&SpriteRockTile, 60, 75},
  {&SpriteRockTile, 60, 90},
  {&SpriteRockTile, 60, 225},
  {&SpriteRockTile, 75, 45},
  {&SpriteRockTile, 75, 60},
  {&SpriteRockTile, 75, 75},
  {&SpriteRockTile, 75, 90},
  {&SpriteRockTile, 75, 225},
  {&SpriteRockTile, 90, 45},
  {&SpriteRockTile, 90, 60},
  {&SpriteRockTile, 90, 75},
  {&SpriteRockTile, 90, 225},
  {&SpriteRockTile, 105, 60},
  {&SpriteRockTile, 105, 75},
  {&SpriteRockTile, 105, 225},
  {&SpriteRockTile, 120, 60},
  {&SpriteRockTile, 120, 75},
  {&SpriteRockTile, 120, 90},
  {&SpriteRockTile, 120, 225},
  {&SpriteRockTile, 135, 60},
  {&SpriteRockTile, 135, 75},
  {&SpriteRockTile, 135, 90},
  {&SpriteRockTile, 135, 225},
  {&SpriteRockTile, 150, 75},
  {&SpriteRockTile, 150, 225},
  {&SpriteRockTile, 165, 225},
  {&SpriteRockTile, 180, 105},
  {&SpriteRockTile, 180, 120},
  {&SpriteRockTile, 180, 210},
  {&SpriteRockTile, 180, 225},
  {&SpriteRockTile, 195, 90},
  {&SpriteRockTile, 195, 105},
  {&SpriteRockTile, 195, 120},
  {&SpriteRockTile, 195, 135},
  {&SpriteRockTile, 195, 210},
  {&SpriteRockTile, 195, 225},
  {&SpriteRockTile, 210, 90},
  {&SpriteRockTile, 210, 105},
  {&SpriteRockTile, 210, 120},
  {&SpriteRockTile, 210, 135},
  {&SpriteRockTile, 210, 210},
  {&SpriteRockTile, 210, 225},
  {&SpriteRockTile, 225, 105},
  {&SpriteRockTile, 225, 120},
  {&SpriteRockTile, 225, 195},
  {&SpriteRockTile, 225, 210},
  {&SpriteRockTile, 225, 225},
  {&SpriteRockTile, 240, 195},
  {&SpriteRockTile, 240, 210},
  {&SpriteRockTile, 240, 225},
  {&SpriteRockTile, 255, 195},
  {&SpriteRockTile, 255, 210},
  {&SpriteRockTile, 255, 225},
  {&SpriteRockTile, 270, 165},
  {&SpriteRockTile, 270, 180},
  {&SpriteRockTile, 270, 195},
  {&SpriteRockTile, 270, 210},
  {&SpriteRockTile, 270, 225},
  {&SpriteRockTile, 285, 135},
  {&SpriteRockTile, 285, 150},
  {&SpriteRockTile, 285, 165},
  {&SpriteRockTile, 285, 180},
  {&SpriteRockTile, 285, 195},
  {&SpriteRockTile, 285, 210},
  {&SpriteRockTile, 285, 225},
  {&SpriteRockTile, 300, 120},
  {&SpriteRockTile, 300, 135},
  {&SpriteRockTile, 300, 150},
  {&SpriteRockTile, 300, 165},
  {&SpriteRockTile, 300, 180},
  {&SpriteRockTile, 300, 195},
  {&SpriteRockTile, 300, 210},
  {&SpriteRockTile, 300, 225}

};

screen_t SCREEN1 = {
  .objects = screen1_tiles,
  .num_objects = ARRAY_SIZE(screen1_tiles)
};
