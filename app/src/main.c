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

#define SLEEP_MS 33        // 30 FPS

// The first 8 bits of each position value represents a subpixel value, allowing for clean movement
// Values should be shifted by SUBPIXEL shift before being passed into LVGL
#define SUBPIXEL_SHIFT 8

#define SCREEN_WIDTH (320 << SUBPIXEL_SHIFT)
#define SCREEN_HEIGHT (240 << SUBPIXEL_SHIFT)

struct {
  int32_t x;
  int32_t y;
} typedef vector2_t;

struct {
  lv_obj_t* player_body;
  vector2_t position;
  vector2_t velocity;
  uint32_t height;
  uint32_t width;
} typedef player_t;

static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));   // Find display device used by LVGL
static lv_obj_t* game_screen;                                                         // LVGL screen on which main game is displayed
static player_t player = {
  NULL,
  {20, 20},
  {0, 2},
  10 << SUBPIXEL_SHIFT,
  10 << SUBPIXEL_SHIFT
};

uint8_t init_game() {
  if (!device_is_ready(display_dev)) {
    printk("Touchscreen device initialization error\n");
    return 1;
  }


  game_screen = lv_obj_create(NULL);                                      // Create LVGL screen

  lv_obj_t* rect = lv_obj_create(game_screen);                            // Create rectangle LVGL object
  lv_obj_set_size(rect, player.width >> SUBPIXEL_SHIFT, player.height >> SUBPIXEL_SHIFT);           // Set size as 10 px by 10 px
  lv_obj_set_pos(rect, player.position.x >> SUBPIXEL_SHIFT, player.position.y >> SUBPIXEL_SHIFT);   // Initial position from top left corner
  lv_obj_set_style_bg_color(rect, lv_color_hex(0xff0000), LV_PART_MAIN);  // Set color as red
  lv_obj_set_style_radius(rect, 0, LV_PART_MAIN);                         // Remove circular corners
  player.player_body = rect;                                              // Set player body as rectangle

  return 0;
}

uint8_t update_game() {
  player.velocity.y += 64;
  switch (joystick_state.h) {
    case JOYSTICK_LEFT:
      player.velocity.x = -256;
      break;
    case JOYSTICK_HORIZONTAL_NEUTRAL:
      player.velocity.x = 0;
      break;
    case JOYSTICK_RIGHT:
      player.velocity.x = 256;
      break;
  }


  player.position.x += player.velocity.x;
  player.position.y += player.velocity.y;
  if (player.position.y > SCREEN_HEIGHT - player.height) {
    player.position.y = SCREEN_HEIGHT - player.height;
    player.velocity.y = 0;
  }

  if (player.position.y == SCREEN_HEIGHT - player.height && button_check_held(BUTTON_ID_A)) {
    player.velocity.y = -1024;
  }

  lv_obj_set_pos(player.player_body, player.position.x >> SUBPIXEL_SHIFT, player.position.y >> SUBPIXEL_SHIFT);


  return 0;
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
  if (0 > init_game()) {
    printk("Error initializing game\n");
    return 0;
  }



  display_blanking_off(display_dev);    // Turn on screen
  lv_screen_load(game_screen);          // Display main game screen

  while(1) {
    lv_timer_handler();
    update_game();

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
