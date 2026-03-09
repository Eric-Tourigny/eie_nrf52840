#pragma once

#include "game_object.h"
#include "controller.h"

// Constants controlling player motion
#define SUBPIXEL_GRAVITY 128
#define PLAYER_HORIZONTAL_SUBPIXEL_SPEED 512
#define JUMP_SPEED 1920
#define FULL_JUMP_DURATION 5 
#define COYOTE_FRAMES 3
#define JUMP_BUTTON BUTTON_ID_B

// Constants based on player sprite
#define PLAYER_WIDTH 11
#define PLAYER_HEIGHT 15

enum __player_state_t {
  PLAYER_GROUNDED,
  PLAYER_FALLING,
  PLAYER_JUMPING
} typedef player_state_t;

struct __player_t {
  game_object_t obj;
  vector2_t vel;
  player_state_t state;
  int16_t frames_airborne;
} typedef player_t;

/*
 * Initialize the player, placing its sprite on `game_screen`
 */
void init_player(lv_obj_t* game_screen);

/*
 * Update the players location and perform collisions for a particular frame
 */
void update_player(game_object_t* colliders, uint32_t num_colliders);
