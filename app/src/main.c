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
#include "game_object.h"

#ifdef __INTELLISENSE__
  #include <modules/lib/gui/lvgl/lvgl.h>
  #include "drivers/BTN/BTN.h"
  #include "drivers/LED/LED.h"
#endif

#define SLEEP_MS 33        // 30 FPS

#define MAX_NUM_GAME_OBJECTS 100

#define SCREEN_WIDTH (320 << SUBPIXEL_SHIFT)
#define SCREEN_HEIGHT (240 << SUBPIXEL_SHIFT)



struct __player_t {
  game_object_t* obj;
  vector2_t vel;
} typedef player_t;

// Sprites
extern const lv_image_dsc_t SpritePlayer; 
extern const lv_image_dsc_t SpriteRockTile;

static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));   // Find display device used by LVGL
static lv_obj_t* game_screen;                                                         // LVGL screen on which main game is displayed

static player_t player;

static game_object_t* game_objects[MAX_NUM_GAME_OBJECTS];




void init_player() {
  player.obj = create_game_object(15, 15 * 10, 15, 15, &SpritePlayer, game_screen);
  player.vel.x = 0;
  player.vel.y = 0;
}

uint8_t init_screen() {
  lv_obj_clean(game_screen);

  init_player();

  game_objects[0] = create_game_object(0, 15 * 15, 15, 15, &SpriteRockTile, game_screen);
  game_objects[1] = create_game_object(15, 15 * 15, 15, 15, &SpriteRockTile, game_screen);
  game_objects[2] = create_game_object(30, 15 * 15, 15, 15, &SpriteRockTile, game_screen);
  game_objects[3] = NULL;

  return 0;
}

uint8_t init_game() {
  if (!device_is_ready(display_dev)) {
    printk("Touchscreen device initialization error\n");
    return 1;
  }

  game_screen = lv_obj_create(NULL);                                      // Create LVGL screen

  return 0;
}

uint8_t update_game() {
  player.vel.y += 64;
  switch (joystick_state.h) {
    case JOYSTICK_LEFT:
      player.vel.x = -256;
      break;
    case JOYSTICK_HORIZONTAL_NEUTRAL:
      player.vel.x = 0;
      break;
    case JOYSTICK_RIGHT:
      player.vel.x = 256;
      break;
  }
  
  int32_t new_x = player.obj->pos.x + player.vel.x;
  int32_t new_y = player.obj->pos.y + player.vel.y;

  for (int i = 0; i < MAX_NUM_GAME_OBJECTS; i++) {
    game_object_t* obj = game_objects[i];
    if (obj == NULL) {
      break;
    }

    if (new_y + player.obj->h > obj->pos.y && new_y < obj->pos.y + obj->h &&
        new_x + player.obj->w > obj->pos.x && new_x < obj->pos.x + obj->w) {
      new_y = obj->pos.y - player.obj->h;
      if (button_check_held(BUTTON_ID_A)) {
        player.vel.y = -1024;
      } else {
        player.vel.y = 0;
      }
    }
  }

  player.obj->pos.x = new_x;
  player.obj->pos.y = new_y;

  lv_obj_set_pos(player.obj->sprite, player.obj->pos.x >> SUBPIXEL_SHIFT, player.obj->pos.y >> SUBPIXEL_SHIFT);


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
  init_screen();
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
