#pragma once

#include "game_object.h"

#define SUBPIXEL_GRAVITY 64
#define PLAYER_HORIZONTAL_SUBPIXEL_SPEED 256

struct __player_t {
  game_object_t obj;
  vector2_t vel;
} typedef player_t;

/*
 * Initialize the player, placing its sprite on `game_screen`
 */
void init_player(lv_obj_t* game_screen);

/*
 * Update the players location and perform collisions for a particular frame
 */
void update_player_location(game_object_t* colliders, uint32_t num_colliders);
