#include "coins.h"
#include "screen.h"
#include <lvgl.h>

/*
 * Update the text displaying how many coins have been collected
 */
static void update_coin_counter();

extern const lv_image_dsc_t SpriteCoin;

// Objects involved in coin display
static lv_obj_t* coin_display;
static lv_obj_t* coins_label;
static lv_obj_t* coin_image;

// The current number of coins the player has collected
static int32_t coin_count = 0;

void update_coin_counter() {
    lv_label_set_text_fmt(coins_label, "%02d", coin_count);
}

void init_coin_display(lv_obj_t* game_screen) {
    // Create the transparent-gray component holding the coin display and place it in the top right
    coin_display = lv_obj_create(lv_layer_top());
    lv_obj_set_style_border_width(coin_display, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(coin_display, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(coin_display, LV_OPA_50, LV_PART_MAIN);
    lv_obj_align(coin_display, LV_ALIGN_TOP_RIGHT, -COINS_LABEL_PX_MARGIN, COINS_LABEL_PX_MARGIN);

    // Size to hold children
    lv_obj_set_style_width(coin_display, LV_SIZE_CONTENT, LV_PART_MAIN);
    lv_obj_set_style_height(coin_display, LV_SIZE_CONTENT, LV_PART_MAIN);
    lv_obj_set_style_pad_hor(coin_display, COINS_LABEL_PX_HORIZONTAL_PADDING, LV_PART_MAIN);
    lv_obj_set_style_pad_ver(coin_display, COINS_LABEL_PX_VERTICAL_PADDING, LV_PART_MAIN);

    // Use flex layout to position children
    lv_obj_set_layout(coin_display, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(coin_display, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(coin_display, 0, LV_PART_MAIN);
    

    // Create the white text displaying the number of coins collected
    coins_label = lv_label_create(coin_display);
    lv_obj_set_style_text_color(coins_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    update_coin_counter();

    // Create the coin sprite image
    coin_image = lv_image_create(coin_display);
    lv_image_set_src(coin_image, &SpriteCoin);
    lv_obj_set_align(coin_image, LV_ALIGN_RIGHT_MID);
}

bool coin_callback(game_object_t* context) {
    // Remove the coin from the screen
    deactivate_game_object(context);
    context->exists = false;

    // Update the coin label
    coin_count += 1;
    update_coin_counter();

    // Don't produce a physical collision
    return false;   
}
