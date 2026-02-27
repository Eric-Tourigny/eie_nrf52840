#pragma once

#include <inttypes.h>
#include <stdbool.h>

#define BUTTON_ID_A 0
#define BUTTON_ID_B 1
#define BUTTON_ID_X 2
#define BUTTON_ID_Y 3

// Numbers representing different positions for horizontal joystick position
typedef enum {
    JOYSTICK_LEFT = 0,
    JOYSTICK_HORIZONTAL_NEUTRAL = 1,
    JOYSTICK_RIGHT = 2
} joystick_horizontal_t;

// Numbers representing different positions of vertical joystick position
typedef enum {
    JOYSTICK_UP = 0,
    JOYSTICK_VERTICAL_NEUTRAL = 1,
    JOYSTICK_DOWN = 2
} joystick_vertical_t;

// Joystick position on horizontal and vertical axes
typedef struct {
    joystick_horizontal_t h;
    joystick_vertical_t v;
} joystick_t;

extern joystick_t joystick_state;           // Current state of the joystick, for global access

/*
 * Begins scanning for the BLE central, triggering a sequence of asynchronous calls that setup the connection and
 * allows different characteristic API calls to be used.
 */
uint8_t init_bluetooth();

/*
 * Returns whether the button `button_id` has been pressed (transition from being up to being down) since this function was last called.
 */
bool button_check_clear_pressed(uint8_t button_id);

/*
 * Returns whether the button `button_id` has been released
 */
bool button_check_clear_released(uint8_t button_id);

/*
 * Returns whether the button `button_id` is currently held down
 */
bool button_check_held(uint8_t button_id);
