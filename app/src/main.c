/*
 * main.c
 */

#include <zephyr/kernel.h>

#include "BTN.h"
#include "LED.h"
#include <zephyr/sys/printk.h>

#define SLEEP_MS 1

int main(void)
{
    if (BTN_init() < 0)
    {
        return 0;
    }
    if (LED_init() < 0)
    {
        return 0;
    }

    led_id LEDs[] = {
        LED0,
        LED1,
        LED2,
        LED3
    };

    uint8_t counter = 0;

    while (1) {
        if (BTN_check_clear_pressed(BTN0))
        {
            counter++;
            uint8_t mask = 1;
            for (int i = 0; i < 4; i++)
            {
                if (counter & mask)
                {
                    LED_set(LEDs[i], LED_ON);
                }
                else
                {
                    LED_set(LEDs[i], LED_OFF);
                }
                mask <<= 1;
            }
            printk("Button 0 pressed!\n");
        }
        k_msleep(SLEEP_MS);
    }
    return 0;
}
