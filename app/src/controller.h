#include <inttypes.h>
#include <stdbool.h>

#define BUTTON_ID_A 0
#define BUTTON_ID_B 1
#define BUTTON_ID_X 2
#define BUTTON_ID_Y 3

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
bool button_check_held(uint8_t button_id);
