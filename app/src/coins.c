#include "coins.h"
#include "screen.h"
#include <lvgl.h>

lv_obj_t* coins_label;
int32_t coin_count;

void init_coin_display(lv_obj_t* game_screen) {
    coins_label = lv_label_create(lv_layer_top());
    lv_obj_align(coins_label, LV_ALIGN_TOP_RIGHT, -COINS_LABEL_PX_PADDING, COINS_LABEL_PX_PADDING);     // Place display in top right
    lv_label_set_text(coins_label, "00");                                                               // Set initial coins as 00
    lv_obj_set_style_bg_color(coins_label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_color(coins_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_pad_all(coins_label, 2, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(coins_label, LV_OPA_50, LV_PART_MAIN);
}

bool coin_callback(game_object_t* context) {
    // Remove the coin from the screen
    deactivate_game_object(context);
    context->exists = false;

    // Update the coin label
    coin_count += 1;
    lv_label_set_text_fmt(coins_label, "%02d", coin_count);

    // Don't produce a physical collision
    return false;   
}
