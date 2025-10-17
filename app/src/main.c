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

    while (1) {
        if (BTN_check_clear_pressed(BTN0))
        {
            LED_toggle(LED0);
            printk("Button 0 pressed!\n");
        }
        k_msleep(SLEEP_MS);
    }
    return 0;
}
