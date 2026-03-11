#include "player.h"
#include "screen.h"


/***************************************************************************************************************************************
 * Variables
 ***************************************************************************************************************************************/

 // Player Sprite Sources
extern const lv_image_dsc_t SpritePlayer;
extern const lv_image_dsc_t SpritePlayerFlipped;

// Player object
static player_t player = {
  {NULL, &SpritePlayer, {TILE_SUBPIXEL_WIDTH, 10 * TILE_SUBPIXEL_HEIGHT}, PLAYER_SUBPIXEL_WIDTH, PLAYER_SUBPIXEL_HEIGHT},
  {0, 0}, 
  PLAYER_FALLING,
  COYOTE_FRAMES + 1   // Don't allow the player to immediately jump
};


/***************************************************************************************************************************************
 * Local Function Declarations
 ***************************************************************************************************************************************/

/*
 * Set player velocity based on player's state and user inputs
 */
void update_player_velocity();


/*
 * Move player, checking for collisions with `colliders`
 */
void update_player_location();


/***************************************************************************************************************************************
 * Local Function Definitions
 ***************************************************************************************************************************************/

void update_player_velocity() {
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

  // Update player depending on their state and determine if they can jump
  bool can_jump = false;
  switch(player.state) {
    case PLAYER_GROUNDED:
      can_jump = true;
      break;
    case PLAYER_FALLING:
      if (player.frames_airborne <= COYOTE_FRAMES) {
        can_jump = true;
      }
      player.frames_airborne += 1;
      break;
    case PLAYER_JUMPING:
      // For variable jump height, check if the player releases the jump button early
      if (button_check_clear_released(JUMP_BUTTON)) {
        // If the player is still moving upwards, half their remaining velocity
        if (player.vel.y < 0) {
          player.vel.y /= 2;
        }
      }
      player.frames_airborne += 1;
      break;
  }

  if (can_jump) {
    if (button_check_clear_pressed(BUTTON_ID_B)) {
      player.state = PLAYER_JUMPING;
      player.vel.y = -JUMP_SPEED;
    }
  }
}

void update_player_location() {
  screen_t* active_screen = get_active_screen();
  if (active_screen == NULL) {
    return;
  }

  // Update player x position ignoring collisions
  player.obj.pos.x += player.vel.x;

  // Check for collisions after applying horizontal motion
  for (int i = 0; i < active_screen->num_objects; i++) {
    game_object_t* obj = &active_screen->objects[i];
    if (check_collision(&player.obj, obj)) {
      if (player.vel.x > 0) {                        // If moving right, collision with the left wall
        player.obj.pos.x = obj->pos.x - player.obj.w;
      }
      else {                                        // Collision with right wall
        player.obj.pos.x = obj->pos.x + obj->w;
      }
    }
  }

  // If player passes screen boundary, move to next screen
  if (player.obj.pos.x < 0) {
    shift_screen(0, -1);
    player.obj.pos.x = SCREEN_SUBPIXEL_WIDTH - TILE_SUBPIXEL_WIDTH - SCREEN_SUBPIXEL_LEFT_EXTRA - SCREEN_SUBPIXEL_RIGHT_EXTRA;
    active_screen = get_active_screen();
  }
  else if (player.obj.pos.x > SCREEN_SUBPIXEL_WIDTH - player.obj.w) {
    shift_screen(0, 1);
    player.obj.pos.x = TILE_SUBPIXEL_WIDTH - player.obj.w + SCREEN_SUBPIXEL_LEFT_EXTRA + SCREEN_SUBPIXEL_RIGHT_EXTRA;
    active_screen = get_active_screen();
  }
  
  // Update player y position ignoring collisions
  player.obj.pos.y += player.vel.y;

  // Check for collisions after applying vertical motion
  bool is_grounded = false;
  for (int i = 0; i < active_screen->num_objects; i++) {
    game_object_t* obj = &active_screen->objects[i];
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

  // If player passes screen boundary, move to next screen
  if (player.obj.pos.y < 0) {
    shift_screen(-1, 0);
    player.obj.pos.y = SCREEN_SUBPIXEL_HEIGHT - SCREEN_SUBPIXEL_TOP_EXTRA - SCREEN_SUBPIXEL_BOTTOM_EXTRA;
  }
  else if (player.obj.pos.y > SCREEN_SUBPIXEL_HEIGHT - player.obj.h) {
    shift_screen(1, 0);
    player.obj.pos.y = SCREEN_SUBPIXEL_TOP_EXTRA + SCREEN_SUBPIXEL_BOTTOM_EXTRA - player.obj.h;
  }
  
 
  if (is_grounded) {                              // Update player state if they are now standing on the ground
    player.state = PLAYER_GROUNDED;
    player.frames_airborne = 0;
    player.vel.y = 0;
  } else if (player.state == PLAYER_GROUNDED) {   // Update player state if they are no longer standing on the ground
    player.state = PLAYER_FALLING;
  }
}

/***************************************************************************************************************************************
 * Global Function Definitions
 ***************************************************************************************************************************************/

void init_player(lv_obj_t* game_screen) {
  activate_game_object(&player.obj, game_screen);
}

void update_player() {
  update_player_velocity();
  update_player_location();

  // Update player sprite position
  lv_obj_set_pos(player.obj.image, player.obj.pos.x >> SUBPIXEL_SHIFT, player.obj.pos.y >> SUBPIXEL_SHIFT);
}
