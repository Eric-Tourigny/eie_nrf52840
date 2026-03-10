#include "screen.h"
#include "player.h"
#include "screen_info.h"

/***************************************************************************************************************************************
 * Variables
 ***************************************************************************************************************************************/

// Import sprites
extern const lv_image_dsc_t SpriteRockTile;
extern const lv_image_dsc_t SpriteGrassTile;
extern const lv_image_dsc_t SpriteDirtTile;

// Currently active screen
static lv_obj_t* game_screen;

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

void init_screen() {
  game_screen = lv_obj_create(NULL);                                                            // Create LVGL screen
  init_player(game_screen);                                                                     // Initialize the player
  lv_obj_set_style_bg_color(game_screen, lv_color_hex(SCREEN_BACKGROUND_COLOR), LV_PART_MAIN);  // Set background color
  activate_screen(STARTING_SCREEN_ROW, STARTING_SCREEN_COL);                                    // Activate the initial screen
  lv_screen_load(game_screen);                                                                  // Put screen on display
}

void activate_screen(int32_t row, int32_t col) {
  current_screen_row = row;
  current_screen_col = col;
  screen_t* screen = screens[row][col];

  for (int i = 0; i < game_object_list.len; i++) {
    lv_obj_delete(game_object_list.game_objects[i].image);
  }
  
  // Create the game objects for the new screen
  for (int i = 0; i < screen->num_objects; i++) {
    const tile_info_t* tile_info = &screen->objects[i];
    init_game_object(
      &game_object_list.game_objects[i], 
      tile_info->x * TILE_PX_WIDTH - SCREEN_PX_LEFT_HIDDEN, 
      tile_info->y * TILE_PX_HEIGHT - SCREEN_PX_TOP_HIDDEN,
      tile_info->w * TILE_PX_WIDTH, 
      tile_info->h * TILE_PX_HEIGHT, 
      tile_info->sprite, 
      game_screen
    );
  }
  game_object_list.len = screen->num_objects;
}

void shift_screen(int32_t row_delta, int32_t col_delta) {
  activate_screen(current_screen_row + row_delta, current_screen_col + col_delta);
}

game_object_list_t* get_active_screen_game_objects() {
  return &game_object_list;
}
