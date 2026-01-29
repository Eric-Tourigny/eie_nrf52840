/*
 * main.c
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1

#define COLUMN_MAX 0x13f
#define ROW_MAX 0xef

#define I_BLUE 0
#define I_GREEN 1
#define I_RED 2

#define CMD_SOFTWARE_RESET 0x01
#define CMD_SLEEP_OUT 0x11
#define CMD_DISPLAY_ON 0x29
#define CMD_COLUMN_ADDRESS_SET 0x2A
#define CMD_ROW_ADDRESS_SET 0x2B
#define CMD_MEMORY_WRITE 0x2C

#define ARDUINO_SPI_NODE DT_NODELABEL(arduino_spi)
#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

static const struct gpio_dt_spec dcx_gpio = GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, dcx_gpios);
static const struct spi_cs_control cs_ctrl = (struct spi_cs_control) {
  .gpio = GPIO_DT_SPEC_GET(ARDUINO_SPI_NODE, cs_gpios),
  .delay = 0u,
};

static const struct device *dev = DEVICE_DT_GET(ARDUINO_SPI_NODE);
static const struct spi_config spi_cfg = {
  .frequency = 1000000,
  .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
  .slave = 0,
  .cs = cs_ctrl
};

static uint8_t pixel_values[COLUMN_MAX + 1][ROW_MAX + 1][3];

static void lcd_cmd(uint8_t cmd, struct spi_buf * data) {
  struct spi_buf cmd_buf[1] = {
    {
      .buf = &cmd,
      .len = 1
    }
  };
  struct spi_buf_set cmd_set = {
    .buffers = cmd_buf,
    .count = 1
  };

  // D/C select must be low to send command
  gpio_pin_set_dt(&dcx_gpio, 0);

  spi_write(dev, &spi_cfg, &cmd_set);

  if (data != NULL) {
    struct spi_buf_set data_set = {
      .buffers=data,
      .count=1
    };

    // D/C select must be high to send command
    gpio_pin_set_dt(&dcx_gpio, 1);

    spi_write(dev, &spi_cfg, &data_set);
  }

}

void set_pixel(uint16_t row, uint16_t col, uint8_t r, uint8_t g, uint8_t b)
{
  uint8_t row_data[] = {row >> 8, row, row >> 8, row};
  uint8_t column_data[] = {col >> 8, col, col >> 8, col};
  uint8_t color_data[] = {b, g, r};

  struct spi_buf row_data_buf = {row_data, 4};
  struct spi_buf column_data_buf = {column_data, 4};
  struct spi_buf color_data_buf = {color_data, 3};

  lcd_cmd(CMD_ROW_ADDRESS_SET, &row_data_buf);
  lcd_cmd(CMD_COLUMN_ADDRESS_SET, &column_data_buf);
  lcd_cmd(CMD_MEMORY_WRITE, &color_data_buf);
}

void write_area(uint16_t row_start, uint16_t row_end, uint16_t col_start, uint16_t col_end, void* values) {
  uint8_t row_data[] = {row_start >> 8, row_start, row_end >> 8, row_end};
  uint8_t column_data[] = {col_start >> 8, col_start, col_end >> 8, col_end};

  struct spi_buf row_data_buf = {row_data, 4};
  struct spi_buf column_data_buf = {column_data, 4};
  struct spi_buf color_values = {values, (row_end - row_start + 1) * (col_end - col_start + 1) * 3};

  lcd_cmd(CMD_COLUMN_ADDRESS_SET, &row_data_buf);
  lcd_cmd(CMD_ROW_ADDRESS_SET, &column_data_buf);
  lcd_cmd(CMD_MEMORY_WRITE, &color_values);
}

int main(void) {
  if (gpio_pin_configure_dt(&dcx_gpio, GPIO_OUTPUT_LOW)) {
    return 0;
  }
  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  lcd_cmd(CMD_SOFTWARE_RESET, NULL);
  k_msleep(120);                      // Software Reset may take 120 ms

  lcd_cmd(CMD_SLEEP_OUT, NULL);
  lcd_cmd(CMD_DISPLAY_ON, NULL);

#if 1
  for (int row = 0; row <= ROW_MAX; row++) {
    for (int col = 0; col <= COLUMN_MAX; col++) {
      pixel_values[col][row][I_BLUE] = 0xff - (row + col) * 0xff / (ROW_MAX + COLUMN_MAX);
    }
  }
#endif
#if 1
  for (int row = 0; row <= ROW_MAX; row++) {
    for (int col = 0; col <= COLUMN_MAX; col++) {
      pixel_values[col][row][I_RED] = 0xff - ((ROW_MAX - row) + col) * 0xff / (ROW_MAX + COLUMN_MAX);
    }
  }
#endif
#if 1
  for (int row = 0; row <= ROW_MAX; row++) {
    for (int col = 0; col <= COLUMN_MAX; col++) {
      pixel_values[col][row][I_GREEN] = col * 0xff / (COLUMN_MAX);
    }
  }
#endif

  write_area(0, ROW_MAX, 0, COLUMN_MAX, pixel_values);



  while(1) {
    k_msleep(SLEEP_MS);
  }
	return 0;
}
