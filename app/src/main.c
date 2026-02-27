/*
 * main.c
 */


#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "LED.h"
#include "BTN.h"
#include "controller.h"

#ifdef __INTELLISENSE__
  #include "drivers/BTN/BTN.h"
  #include "drivers/LED/LED.h"
#endif


#define SLEEP_MS 1

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

  while(1) {
    if (button_check_clear_pressed(BUTTON_ID_A)) {
      LED_toggle(LED0);
    }
    if (button_check_clear_released(BUTTON_ID_B)) {
      LED_toggle(LED1);
    }
    if (button_check_clear_pressed(BUTTON_ID_X) || button_check_clear_released(BUTTON_ID_X)) {
      LED_toggle(LED2);
    }
    LED_set(LED3, button_check_held(BUTTON_ID_Y));

    k_msleep(SLEEP_MS);
  }

	return 0;
}
