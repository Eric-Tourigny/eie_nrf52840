#include "screen.h"
#include <zephyr/sys/util.h>

extern const lv_image_dsc_t SpriteRockTile;

static game_object_t* game_objects[MAX_NUM_GAME_OBJECTS + 1];

game_object_t** activate_screen(screen_t* screen, lv_obj_t* parent) {
  // Free the memory used by the last screen, if any
  for (int i = 0; ; i++) {
    game_object_t* obj = game_objects[i];
    if (obj == NULL) {
      break;
    }
    destroy_game_object(obj);
  }
  
  // Create the game objects for the new screen
  int i;
  for (i = 0; i < screen->num_objects; i++) {
    const tile_info_t* tile_info = &screen->objects[i];
    game_objects[i] = create_game_object(tile_info->x, tile_info->y, TILE_WIDTH, TILE_HEIGHT, tile_info->sprite, parent);
  }

  // Place a null terminator on the game objects array, to indicate its end
  game_objects[i] = NULL;

  return game_objects;
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
  {&SpriteRockTile, 90, 60}
};

screen_t SCREEN1 = {
  .objects = screen1_tiles,
  .num_objects = ARRAY_SIZE(screen1_tiles)
};
