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
  player.state = PLAYER_FALLING;
  player.frames_airborne = 0;
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
  
  // Update player x position ignoring collisions
  player.obj.pos.x += player.vel.x;

  // Check for collisions after applying horizontal motion
  for (int i = 0; i < num_colliders; i++) {
    game_object_t* obj = &colliders[i];
    if (check_collision(&player.obj, obj)) {
      if (player.vel.x > 0) {                        // If moving right, collision with the left wall
        player.obj.pos.x = obj->pos.x - player.obj.w;
      }
      else {                                        // Collision with right wall
        player.obj.pos.x = obj->pos.x + obj->w;
      }
    }
  }
  
  // Update player y position ignoring collisions
  player.obj.pos.y += player.vel.y;

  // Check for collisions after applying vertical motion
  bool is_grounded = false;
  for (int i = 0; i < num_colliders; i++) {
    game_object_t* obj = &colliders[i];
    if (check_collision(&player.obj, obj)) {
      if (player.vel.y > 0) {             // If moving down, must be standing on the ground
        is_grounded = true;
        player.obj.pos.y = obj->pos.y - player.obj.h;
      }
      else {                             // Hitting the ceiling
        player.vel.y = 0;
        player.obj.pos.y = obj->pos.y + obj->h;
      }
    }
  }

  bool can_jump = false;

  // Update player state
  if (is_grounded) {
    player.state = PLAYER_GROUNDED;
    player.frames_airborne = 0;
    player.vel.y = 0;
    can_jump = true;
  } else {
    switch(player.state) {
      case PLAYER_GROUNDED:
        player.state = PLAYER_FALLING;
        break;
      case PLAYER_FALLING:
        player.frames_airborne += 1;
        if (player.frames_airborne <= COYOTE_FRAMES) {
          can_jump = true;
        }
        break;
      case PLAYER_JUMPING:
        player.frames_airborne += 1;
        break;
    }
  }

  if (can_jump) {
    if (button_check_clear_pressed(BUTTON_ID_B)) {
      player.state = PLAYER_JUMPING;
      player.vel.y = -JUMP_SPEED;
    }
  }

  // Update player sprite position
  lv_obj_set_pos(player.obj.image, player.obj.pos.x >> SUBPIXEL_SHIFT, player.obj.pos.y >> SUBPIXEL_SHIFT);
}
