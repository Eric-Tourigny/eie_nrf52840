/*
 * main.c
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/sys/printk.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1

#define COLUMN_MAX 0x13f
#define ROW_MAX 0xef

// Setup LCD
#define DEVICE_NODE DT_CHOSEN(zephyr_display)
static const struct device *display_dev = DEVICE_DT_GET(DEVICE_NODE);

// Memory allocated for writing to LCD
static uint8_t pixel_values[COLUMN_MAX + 1][ROW_MAX + 1][3];


void write_area(uint16_t row_start, uint16_t row_end, uint16_t col_start, uint16_t col_end, void* values) {
  uint16_t row_width = row_end - row_start + 1;
  uint16_t col_width = col_end - col_start + 1;
  struct display_buffer_descriptor bufferDescriptor = {
      (row_width) * (col_width),
      row_width,
      col_width,
      0,
      true
  };
  int result = display_write(display_dev, row_start, col_start, &bufferDescriptor, values);
  printk("%d", result);
}

int main(void) {
  // Check Display Ready
  if (!device_is_ready(display_dev)) {
    return 0;
  }
  // Initialize buttons
  if (0 > BTN_init()) {
    return 0;
  }
  // Initialize LEDs
  if (0 > LED_init()) {
    return 0;
  }

  display_blanking_off(display_dev);
  display_set_orientation(display_dev, DISPLAY_ORIENTATION_NORMAL);

#if 0
  for (int row = 0; row <= ROW_MAX; row++) {
    for (int col = 0; col <= COLUMN_MAX; col++) {
      pixel_values[col][row][0] = 0xff - ((ROW_MAX - row) + col) * 0xff / (ROW_MAX + COLUMN_MAX);
    }
  }
#endif
#if 0
  for (int row = 0; row <= ROW_MAX; row++) {
    for (int col = 0; col <= COLUMN_MAX; col++) {
      pixel_values[col][row][1] = col * 0xff / (COLUMN_MAX);
    }
  }
#endif
#if 1
  for (int row = 0; row <= ROW_MAX; row++) {
    for (int col = 0; col <= COLUMN_MAX; col++) {
      pixel_values[col][row][2] = 0xff - (((row + col) * 0xff) / (ROW_MAX + COLUMN_MAX));
    }
  }
#endif


  write_area(0, ROW_MAX, 0, COLUMN_MAX, pixel_values);

  while(1) {
    k_msleep(SLEEP_MS);
  }
	return 0;
}
