/*
 * main.c
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

#define SLEEP_TIME_MS 1000

#define SWO_NODE DT_ALIAgS(sw0)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SWO_NODE, gpios);


int main(void)
{
    int ret;

    if (!gpio_is_ready_dt(&button))
    {
        return 0;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0)
    {
        return 0;
    }

    while (1) {
        ret = gpio_pin_get_dt(&button);
        if (ret > 0)
        {
            printk("Pressed!\n");
        }
        k_msleep(SLEEP_TIME_MS);
    }
    return 0;
}
