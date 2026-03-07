#include "player.h"
#include "controller.h"

// Player Sprite Source
extern const lv_image_dsc_t SpritePlayer; 

// Player object
static player_t player;


void init_player(lv_obj_t* game_screen) {
  init_game_object(&player.obj, 15, 15 * 10, 15, 15, &SpritePlayer, game_screen);
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
  
  // Apply velocity to update player position
  int32_t new_x = player.obj.pos.x + player.vel.x;
  int32_t new_y = player.obj.pos.y + player.vel.y;

  // Check for collisions which each supplied game object
  for (int i = 0; i < num_colliders; i++) {
    game_object_t* obj = &colliders[i];

    // Check for player standing on ground
    if (new_y + player.obj.h > obj->pos.y && new_y < obj->pos.y + obj->h &&
        new_x + player.obj.w > obj->pos.x && new_x < obj->pos.x + obj->w) {
      new_y = obj->pos.y - player.obj.h;
      if (button_check_held(BUTTON_ID_B)) {
        player.vel.y = -1024;
      } else {
        player.vel.y = 0;
      }
    }
  }

  // Update internal player position
  player.obj.pos.x = new_x;
  player.obj.pos.y = new_y;

  // Update player sprite position
  lv_obj_set_pos(player.obj.sprite, player.obj.pos.x >> SUBPIXEL_SHIFT, player.obj.pos.y >> SUBPIXEL_SHIFT);
}