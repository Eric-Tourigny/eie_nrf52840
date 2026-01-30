/**
 * @file main.c
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include <lvgl.h>

#include "BTN.h"
#include "LED.h"
#include "lv_data_obj.h"

#ifdef __INTELLISENSE__
#include <modules/lib/gui/lvgl/lvgl.h>
#endif

#define SLEEP_MS 1
#define NUM_BUTTONS 4

static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static lv_obj_t *screen = NULL;

int main(void) {
  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  // Check if touchscreen device is ready
  if (!device_is_ready(display_dev)) {
    return 0;
  }
  
  // Create LVGL screen
  screen = lv_screen_active();
  if (screen == NULL) {
    return 0;
  }
  
  for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
    lv_obj_t *button = lv_button_create(screen);    // Create button as child of screen
    lv_obj_align(button, LV_ALIGN_CENTER, i % 2 ? 50 : -50, i < 2 ? -20 : 20);

    lv_obj_t *label = lv_label_create(button);      // Create label for button
    char label_text[10];                            // Text for coin's label
    sprintf(label_text, "Button %u", i + 1);        // Write text into buffer
    lv_label_set_text(label, label_text);           // Set label text
  }


  display_blanking_off(display_dev);
  while (1) {
    lv_timer_handler();
    k_msleep(SLEEP_MS);
  }

  return 0;
}
