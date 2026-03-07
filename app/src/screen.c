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
    {&SpriteRockTile, 0, 15 * 15},
    {&SpriteRockTile, 15, 15 * 15}
};

screen_t SCREEN1 = {
  .objects = screen1_tiles,
  .num_objects = ARRAY_SIZE(screen1_tiles)
};
