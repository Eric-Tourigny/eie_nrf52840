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

#define SLEEP_MS 100
#define NUM_BUTTONS 4

static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static lv_obj_t* buttonScreen;
static lv_obj_t* imageScreen;

extern const lv_image_dsc_t UCalgary_Vertical_logo_colour_1;

void button_callback(lv_event_t *event) {
  lv_screen_load(imageScreen);
  lv_obj_t* data_obj = (lv_obj_t*) lv_event_get_user_data(event);
  uint8_t* user_data = (uint8_t*) lv_data_obj_get_data_ptr(data_obj);
  printk("Button %u Pressed\n", *user_data);
}

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

  buttonScreen = lv_obj_create(NULL);
  imageScreen = lv_obj_create(NULL);
  
  for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
    lv_obj_t *button = lv_button_create(buttonScreen);                                        // Create button as child of screen
    lv_obj_align(button, LV_ALIGN_CENTER, i % 2 ? 50 : -50, i < 2 ? -20 : 20);          // Align in 2x2 grid
    lv_obj_t* user_data = lv_data_obj_create_alloc_assign(button, &i, sizeof(i));       // Associate button number information with button
    lv_obj_add_event_cb(button, button_callback, LV_EVENT_CLICKED, user_data);          // Add callback function on button press

    lv_obj_t *label = lv_label_create(button);      // Create label for button
    char label_text[11];                            // Text for coin's label
    sprintf(label_text, "Button %u", i);        // Write text into buffer
    lv_label_set_text(label, label_text);           // Set label text
  }

  lv_obj_t* image = lv_image_create(imageScreen);
  lv_image_set_src(image, &UCalgary_Vertical_logo_colour_1);
  lv_obj_align(image, LV_ALIGN_CENTER, 0, 0);

  display_blanking_off(display_dev);

  lv_screen_load(buttonScreen);

  uint8_t y = 0;
  while (1) {
    lv_obj_align(image, LV_ALIGN_CENTER, 0, y);
    y++;
    lv_timer_handler();
    k_msleep(SLEEP_MS);
  }

  return 0;
}
