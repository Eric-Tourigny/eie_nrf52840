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
    k_msleep(SLEEP_MS);
  }
	return 0;
}
