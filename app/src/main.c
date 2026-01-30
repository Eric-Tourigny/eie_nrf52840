/*
 * main.c
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/i2c.h>
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

// LCD Command Codes
#define CMD_SOFTWARE_RESET 0x01
#define CMD_SLEEP_OUT 0x11
#define CMD_DISPLAY_ON 0x29
#define CMD_COLUMN_ADDRESS_SET 0x2A
#define CMD_ROW_ADDRESS_SET 0x2B
#define CMD_MEMORY_WRITE 0x2C

// Setup LCD
#define ARDUINO_SPI_NODE DT_NODELABEL(arduino_spi)
#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

static const struct gpio_dt_spec dcx_gpio = GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, dcx_gpios);
static const struct spi_cs_control cs_ctrl = (struct spi_cs_control) {
  .gpio = GPIO_DT_SPEC_GET(ARDUINO_SPI_NODE, cs_gpios),
  .delay = 0u,
};

static const struct device *spi_dev = DEVICE_DT_GET(ARDUINO_SPI_NODE);
static const struct spi_config spi_cfg = {
  .frequency = 1000000,
  .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
  .slave = 0,
  .cs = cs_ctrl
};

// Setup touchscreen
#define ARDUINO_I2C_NODE DT_NODELABEL(arduino_i2c)
static const struct device* i2c_dev = DEVICE_DT_GET(ARDUINO_I2C_NODE);

// Touchscreen registers
#define TD_STATUS 0x02      // Number of touch points
#define P1_XH 0x03          // First touch x position high, touch event
#define P1_XL 0x04          // First touch x position low
#define P1_YH 0x05          // First touch y position high
#define P1_YL 0x06          // First touch y position low

// Touchscreen defines
#define P1_XH_EVENT_MASK 0xc0
#define TOUCH_UPPER_MASK 0x4
#define TOUCH_UPPER_SHIFT 8
#define ADDR 0x38

struct lcd_position {
  uint8_t x_pos_h;
  uint8_t x_pos_l;
  uint8_t y_pos_h;
  uint8_t y_pos_l;
};


// Memory allocated for writing to LCD
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

  spi_write(spi_dev, &spi_cfg, &cmd_set);

  if (data != NULL) {
    struct spi_buf_set data_set = {
      .buffers=data,
      .count=1
    };

    // D/C select must be high to send command
    gpio_pin_set_dt(&dcx_gpio, 1);

    spi_write(spi_dev, &spi_cfg, &data_set);
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

void touch_control_cmd_rsp(uint8_t cmd, void* rsp, int size) {
  struct i2c_msg cmd_rsp_msg[2] = {
    {&cmd, 1, I2C_MSG_WRITE},
    {rsp, size, I2C_MSG_RESTART | I2C_MSG_READ | I2C_MSG_STOP}
  };
  i2c_transfer(i2c_dev, cmd_rsp_msg, 2, ADDR);
}

int main(void) {
  // Configure SPI pin
  if (gpio_pin_configure_dt(&dcx_gpio, GPIO_OUTPUT_LOW)) {
    return 0;
  }
  // Check I2C Device Ready
  if (!device_is_ready(i2c_dev)) {
    return 0;
  }
  // Configure I2C device
  if (0 > i2c_configure(i2c_dev, I2C_SPEED_SET(I2C_SPEED_FAST) | I2C_MODE_CONTROLLER)) {
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
    uint8_t touch_status;
    touch_control_cmd_rsp(TD_STATUS, &touch_status, 1);
    if (touch_status == 1)  // One touch event is occurring
    {
      struct lcd_position position_data;

      touch_control_cmd_rsp(P1_XH, &position_data, 4);

      uint16_t x_pos = ((position_data.x_pos_h & TOUCH_UPPER_MASK) << TOUCH_UPPER_SHIFT) | position_data.x_pos_l;
      uint16_t y_pos = ((position_data.y_pos_h & TOUCH_UPPER_MASK) << TOUCH_UPPER_SHIFT) | position_data.y_pos_l;

      printk("Touch at (%u, %u)", x_pos, y_pos);
      
      k_msleep(SLEEP_MS);
    }

  }
	return 0;
}
