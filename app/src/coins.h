#pragma once

#include "game_object.h"
#include "lvgl.h"

#define COINS_LABEL_PX_PADDING 10

void init_coin_display(lv_obj_t* game_screen);

bool coin_callback(game_object_t* context);
