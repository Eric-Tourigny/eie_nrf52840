#include <inttypes.h>
#include <stdbool.h>

#define BUTTON_ID_A 0
#define BUTTON_ID_B 1
#define BUTTON_ID_X 2
#define BUTTON_ID_Y 3

uint8_t init_bluetooth();
bool button_check_clear_pressed(uint8_t button_id);
bool button_check_clear_released(uint8_t button_id);
bool button_check_held(uint8_t button_id);
