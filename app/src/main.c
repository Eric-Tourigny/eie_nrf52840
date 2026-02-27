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

#ifdef __INTELLISENSE__
  #include <modules/lib/gui/lvgl/lvgl.h>
  #include "drivers/BTN/BTN.h"
  #include "drivers/LED/LED.h"
#endif

#define SLEEP_MS 1

static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));   // Find display device used by LVGL
static lv_obj_t* game_screen = NULL;                                                  // LVGL screen on which main game is displayed

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
  if (!device_is_ready(display_dev)) {
    printk("Touchscreen device initialization error\n");
    return 0;
  }

  game_screen = lv_obj_create(NULL);                                      // Create LVGL screen

  lv_obj_t* rect = lv_obj_create(game_screen);                            // Create rectangle LVGL object
  lv_obj_set_size(rect, 10, 10);                                          // Set size as 10 px by 10 px
  lv_obj_set_pos(rect, 10, 10);                                           // Initial position from top left corner
  lv_obj_set_style_bg_color(rect, lv_color_hex(0xff0000), LV_PART_MAIN);  // Set color as red
  lv_obj_set_style_radius(rect, 0, LV_PART_MAIN);                         // Remove circular corners

  display_blanking_off(display_dev);    // Turn on screen
  lv_screen_load(game_screen);          // Display main game screen

  while(1) {
    lv_timer_handler();

    if (button_check_clear_pressed(BUTTON_ID_A)) {
      if (joystick_state.h == JOYSTICK_LEFT) {
        LED_toggle(LED0);
      } else if (joystick_state.h == JOYSTICK_RIGHT) {
        LED_toggle(LED3);
      }
      
      if (joystick_state.v == JOYSTICK_DOWN) {
        LED_toggle(LED2);
      } else if (joystick_state.v == JOYSTICK_UP) {
        LED_toggle(LED1);
      }
    }

    k_msleep(SLEEP_MS);
  }

	return 0;
}
