/*
 * main.c
 */


#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include <lvgl.h>

#include "LED.h"
#include "BTN.h"
#include "lv_data_obj.h"

#include "controller.h"
#include "game_object.h"
#include "player.h"
#include "screen.h"

#ifdef __INTELLISENSE__
  #include <modules/lib/gui/lvgl/lvgl.h>
  #include "drivers/BTN/BTN.h"
  #include "drivers/LED/LED.h"
#endif

#define SLEEP_MS 33        // 30 FPS


#define SCREEN_WIDTH (320 << SUBPIXEL_SHIFT)
#define SCREEN_HEIGHT (240 << SUBPIXEL_SHIFT)


static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));   // Find display device used by LVGL
static lv_obj_t* game_screen;                                                         // LVGL screen on which main game is displayed
static lv_obj_t** colliders;                                                           // Other game objects requiring updates on the screen

void init_lvgl() {
  if (!device_is_ready(display_dev)) {
    printk("Touchscreen device initialization error\n");
  }
  game_screen = lv_obj_create(NULL);      // Create LVGL screen
  lv_screen_load(game_screen);            // Display main game screen
  display_blanking_off(display_dev);      // Turn on screen
}

void init_game() {
  lv_obj_clean(game_screen);
  init_player(game_screen);
  colliders = activate_screen(&SCREEN1, game_screen);
}

void update_game() {
  update_player_location(colliders);
}

int main(void) {
  if (0 > BTN_init()) {
    printk("Button initialization error\n");
    return 0;
  }
  if (0 > LED_init()) {
    printk("LED initialization error\n");
    return 0;
  }
  if (0 > init_bluetooth()) {
    printk("Bluetooth initialization error\n");
    return 0;
  }

  init_lvgl();
  init_game();

  while(1) {
    lv_timer_handler();
    update_game();

    k_msleep(SLEEP_MS);
  }

	return 0;
}
