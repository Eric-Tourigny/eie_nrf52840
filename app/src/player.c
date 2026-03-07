#include "player.h"
#include "controller.h"
#include <zephyr/sys/printk.h>

// Player Sprite Source
extern const lv_image_dsc_t SpritePlayer; 

// Player object
static player_t player;


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
      break;
    case JOYSTICK_HORIZONTAL_NEUTRAL:
      player.vel.x = 0;
      break;
    case JOYSTICK_RIGHT:
      player.vel.x = PLAYER_HORIZONTAL_SUBPIXEL_SPEED;
      break;
  }
  
  // Find player x position ignoring collisions
  int32_t new_x = player.obj.pos.x + player.vel.x;

  // Check for collisions after applying horizontal motion
  for (int i = 0; i < num_colliders; i++) {
    game_object_t* obj = &colliders[i];

    bool is_right_of_left        = new_x + player.obj.w > obj->pos.x;
    bool is_left_of_left_hitbox  = new_x + player.obj.w < obj->pos.x + HORIZONTAL_HITBOX_SUBPIXEL_THICKNESS;
    bool was_left_of_left        = player.obj.pos.x + player.obj.w <= obj->pos.x;

    bool is_left_of_right        = new_x < obj->pos.x + obj->w;
    bool is_right_of_right_hitbox= new_x > obj->pos.x + obj->w - HORIZONTAL_HITBOX_SUBPIXEL_THICKNESS;
    bool was_right_of_right      = player.obj.pos.x >= obj->pos.x + obj->w;

    bool is_below_top            = player.obj.pos.y + player.obj.h > obj->pos.y;
    bool is_above_bottom         = player.obj.pos.y < obj->pos.y + obj->h;
    bool within_vertical_range   = is_below_top && is_above_bottom;


    if (is_right_of_left && is_left_of_left_hitbox && within_vertical_range && was_left_of_left)            // Collision with the left wall
    {
      new_x = obj->pos.x - player.obj.w;
      printk("left hitbox");
    }
    else if (is_left_of_right && is_right_of_right_hitbox && within_vertical_range && was_right_of_right)   // Collision with right wall
    {
      new_x = obj->pos.x + obj->w;
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
    bool is_above_top_hitbox     = new_y + player.obj.h < obj->pos.y + VERTICAL_HITBOX_SUBPIXEL_THICKNESS;

    bool is_above_bottom         = new_y < obj->pos.y + obj->h;
    bool is_below_bottom_hitbox  = new_y > obj->pos.y + obj->h - VERTICAL_HITBOX_SUBPIXEL_THICKNESS;

    if (is_below_top && is_above_top_hitbox && within_horizontal_range)                                // Standing on the ground
    {
      if (button_check_held(BUTTON_ID_B)) {
        player.vel.y = -1024;
      } else {
        player.vel.y = 0;
      }
      new_y = obj->pos.y - player.obj.h;
    }
    else if (is_above_bottom && is_below_bottom_hitbox && within_horizontal_range)                          // Hitting the ceiling
    {
      player.vel.y = 0;
      new_y = obj->pos.y + obj->h;
    }
  }

  // Update actual player y position
  player.obj.pos.y = new_y;

  // Update player sprite position
  lv_obj_set_pos(player.obj.sprite, player.obj.pos.x >> SUBPIXEL_SHIFT, player.obj.pos.y >> SUBPIXEL_SHIFT);
}
