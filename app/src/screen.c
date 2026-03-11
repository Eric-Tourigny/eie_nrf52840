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

// LVGL screen for the game
static lv_obj_t* game_screen;

// Grid of available screens
extern screen_t *screens[NUM_SCREEN_ROWS][NUM_SCREEN_COLS];
static int32_t current_screen_row;
static int32_t current_screen_col;

// Currently active screen
static screen_t* current_screen = NULL;

/***************************************************************************************************************************************
 * Global Function Definitions
 ***************************************************************************************************************************************/

void init_screen() {
  game_screen = lv_obj_create(NULL);                                                            // Create LVGL screen
  init_player(game_screen);                                                                     // Initialize the player
  lv_obj_set_style_bg_color(game_screen, lv_color_hex(SCREEN_BACKGROUND_COLOR), LV_PART_MAIN);  // Set background color
  lv_obj_remove_flag(game_screen, LV_OBJ_FLAG_SCROLLABLE);                                      // Don't display scroll bars
  activate_screen(STARTING_SCREEN_ROW, STARTING_SCREEN_COL);                                    // Activate the initial screen
  lv_screen_load(game_screen);                                                                  // Put screen on display
}

void activate_screen(int32_t row, int32_t col) {
  // Deactivate objects on the old screen
  if (current_screen != NULL){
    for (int i = 0; i < current_screen->num_objects; i++) {
      deactivate_game_object(&current_screen->objects[i]);
    }
  }
  
  // Change the currently active screen
  current_screen_row = row;
  current_screen_col = col;
  current_screen = screens[row][col];

  // Activate objects on the new screen
  for (int i = 0; i < current_screen->num_objects; i++) {
    activate_game_object(&current_screen->objects[i], game_screen);
  }
}

void shift_screen(int32_t row_delta, int32_t col_delta) {
  activate_screen(current_screen_row + row_delta, current_screen_col + col_delta);
}

screen_t* get_active_screen() {
  return current_screen;
}
