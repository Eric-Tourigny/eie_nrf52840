#include "screen.h"

/***************************************************************************************************************************************
 * Variables
 ***************************************************************************************************************************************/

// Import sprites
extern const lv_image_dsc_t SpriteRockTile;
extern const lv_image_dsc_t SpriteGrassTile;
extern const lv_image_dsc_t SpriteDirtTile;

// List tracking currently active game objects
static game_object_t game_objects[MAX_NUM_GAME_OBJECTS + 1];
static game_object_list_t game_object_list = {.game_objects = game_objects, .len = 0};

// Grid of available screens
extern screen_t *screens[NUM_SCREEN_ROWS][NUM_SCREEN_COLS];
static int32_t current_screen_row;
static int32_t current_screen_col;

/***************************************************************************************************************************************
 * Global Function Definitions
 ***************************************************************************************************************************************/

void activate_screen(int32_t row, int32_t col, lv_obj_t* parent) {
  screen_t* screen = screens[row][col];

  // Set parent's background color
  lv_obj_set_style_bg_color(parent, lv_color_hex(SCREEN_BACKGROUND_COLOR), LV_PART_MAIN);
  
  // Create the game objects for the new screen
  for (int i = 0; i < screen->num_objects; i++) {
    const tile_info_t* tile_info = &screen->objects[i];
    init_game_object(&game_object_list.game_objects[i], tile_info->x, tile_info->y, TILE_WIDTH, TILE_HEIGHT, tile_info->sprite, parent);
  }
  game_object_list.len = screen->num_objects;
}

void shift_screen(int32_t row_delta, int32_t col_delta, lv_obj_t* parent) {
  activate_screen(current_screen_row + row_delta, current_screen_col + col_delta, parent);
}

game_object_list_t* get_active_screen_game_objects() {
  return &game_object_list;
}
