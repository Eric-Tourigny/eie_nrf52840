#pragma once

#include "game_object.h"

// Constants controlling player motion
#define SUBPIXEL_GRAVITY 128
#define PLAYER_HORIZONTAL_SUBPIXEL_SPEED 512
#define JUMP_SPEED 2048

#define VERTICAL_HITBOX_SUBPIXEL_THICKNESS (10 << SUBPIXEL_SHIFT)
#define HORIZONTAL_HITBOX_SUBPIXEL_THICKNESS (10 << SUBPIXEL_SHIFT)

// Constants based on player sprite
#define PLAYER_WIDTH 11
#define PLAYER_HEIGHT 15

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
