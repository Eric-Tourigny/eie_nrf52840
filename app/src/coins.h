#pragma once

#include "game_object.h"
#include "lvgl.h"

#define COINS_LABEL_PX_MARGIN 10
#define COINS_LABEL_PX_HORIZONTAL_PADDING 5
#define COINS_LABEL_PX_VERTICAL_PADDING 4

/*
 * Create the component displaying the number of coins collected
 */
void init_coin_display();

/*
 * Destroy the coin `context` and increment the coin counter
 */
bool coin_callback(game_object_t* context);
