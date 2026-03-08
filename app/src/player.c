#include "player.h"
#include "controller.h"
#include <zephyr/sys/printk.h>


/***************************************************************************************************************************************
 * Variables
 ***************************************************************************************************************************************/

 // Player Sprite Sources
extern const lv_image_dsc_t SpritePlayer;
extern const lv_image_dsc_t SpritePlayerFlipped;

// Player object
static player_t player;


/***************************************************************************************************************************************
 * Global Function Definitions
 ***************************************************************************************************************************************/

void init_player(lv_obj_t* game_screen) {
  init_game_object(&player.obj, 15, 15 * 10, PLAYER_WIDTH, PLAYER_HEIGHT, &SpritePlayer, game_screen);
  player.vel.x = 0;
  player.vel.y = 0;
}

void update_player_location(game_object_t* colliders, uint32_t num_colliders) {
  // Apply force of gravity
  player.vel.y += SUBPIXEL_GRAVITY;

  // Move player based on joystick direction
  switch (joystick_state.h) {
    case JOYSTICK_LEFT:
      player.vel.x = -PLAYER_HORIZONTAL_SUBPIXEL_SPEED;
      set_game_object_sprite(&player.obj, &SpritePlayerFlipped);
      break;
    case JOYSTICK_HORIZONTAL_NEUTRAL:
      player.vel.x = 0;
      break;
    case JOYSTICK_RIGHT:
      set_game_object_sprite(&player.obj, &SpritePlayer);
      player.vel.x = PLAYER_HORIZONTAL_SUBPIXEL_SPEED;
      break;
  }
  
  // Find player x position ignoring collisions
  int32_t new_x = player.obj.pos.x + player.vel.x;

  // Check for collisions after applying horizontal motion
  for (int i = 0; i < num_colliders; i++) {
    game_object_t* obj = &colliders[i];

    bool is_right_of_left        = new_x + player.obj.w > obj->pos.x;
    bool is_left_of_right        = new_x < obj->pos.x + obj->w;
    bool within_horizontal_range = is_right_of_left && is_left_of_right;

    bool is_below_top            = player.obj.pos.y + player.obj.h > obj->pos.y;
    bool is_above_bottom         = player.obj.pos.y < obj->pos.y + obj->h;
    bool within_vertical_range   = is_below_top && is_above_bottom;

    bool is_moving_right         = player.vel.x > 0;

    if (within_horizontal_range && within_vertical_range)            
    {
      if (is_moving_right)                                      // Collision with the left wall
      {
        new_x = obj->pos.x - player.obj.w;
      }
      else                                                      // Collision with right wall
      {
        new_x = obj->pos.x + obj->w;
      }
    }
  }

  // Update actual player x position
  player.obj.pos.x = new_x;
  
  // Find player y position ignoring collisions
  int32_t new_y = player.obj.pos.y + player.vel.y;

  // Check for collisions after applying vertical motion
  for (int i = 0; i < num_colliders; i++) {
    game_object_t* obj = &colliders[i];

    bool is_right_of_left        = player.obj.pos.x + player.obj.w > obj->pos.x;
    bool is_left_of_right        = player.obj.pos.x < obj->pos.x + obj->w;
    bool within_horizontal_range = is_right_of_left && is_left_of_right;

    bool is_below_top            = new_y + player.obj.h > obj->pos.y;
    bool is_above_bottom         = new_y < obj->pos.y + obj->h;
    bool within_vertical_range   = is_below_top && is_above_bottom;

    bool is_moving_down          = player.vel.y > 0;

    if (within_horizontal_range && within_vertical_range)                                
    {
      if (is_moving_down)               // Standing on the ground
      {
        if (button_check_clear_pressed(BUTTON_ID_B)) {
          player.vel.y = -JUMP_SPEED;
        } else {
          player.vel.y = 0;
        }
        new_y = obj->pos.y - player.obj.h;
      }
      else                              // Hitting the ceiling
      {
        player.vel.y = 0;
        new_y = obj->pos.y + obj->h;
      }
    }
  }

  // Update actual player y position
  player.obj.pos.y = new_y;

  // Update player sprite position
  lv_obj_set_pos(player.obj.image, player.obj.pos.x >> SUBPIXEL_SHIFT, player.obj.pos.y >> SUBPIXEL_SHIFT);
}
