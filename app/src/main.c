/*
 * main.c
 */

#include "controller.h"
#include "BTN.h"
#include "LED.h"

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>


#define SLEEP_MS 1

int main(void) {
  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }
  if (0 > init_bluetooth()) {
    return 0;
  }

  while(1) {
    k_msleep(SLEEP_MS);
  }
	return 0;
}
