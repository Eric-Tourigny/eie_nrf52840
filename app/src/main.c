/*
 * main.c
 */


#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include "LED.h"
#include "BTN.h"
#include "lv_data_obj.h"

#include "controller.h"
#include "game_object.h"
#include "player.h"
#include "screen.h"

#ifdef __INTELLISENSE__
  #include "drivers/BTN/BTN.h"
  #include "drivers/LED/LED.h"
#endif

#define SLEEP_MS 33        // 30 FPS

static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));   // Find display device used by LVGL

void init_display() {
  if (!device_is_ready(display_dev)) {
    printk("Touchscreen device initialization error\n");
  }
  display_blanking_off(display_dev);      // Turn on screen
}

void update_game() {
  update_player();
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

  init_display();
  init_screen();

  while(1) {
    lv_timer_handler();
    update_game();

    k_msleep(SLEEP_MS);
  }

	return 0;
}
