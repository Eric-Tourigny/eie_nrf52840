#include "screen.h"
#include <zephyr/sys/util.h>

extern const lv_image_dsc_t SpriteRockTile;

static game_object_t game_objects[MAX_NUM_GAME_OBJECTS + 1];
static game_object_list_t game_object_list = {.game_objects = game_objects, .len = 0};

game_object_list_t* activate_screen(screen_t* screen, lv_obj_t* parent) {
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
};

screen_t SCREEN1 = {
  .objects = screen1_tiles,
  .num_objects = ARRAY_SIZE(screen1_tiles)
};
