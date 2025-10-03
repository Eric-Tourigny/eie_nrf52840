/*
 * main.c
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <stdlib.h>

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

static const struct gpio_dt_spec LED0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec LED1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec LED2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec LED3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

const struct gpio_dt_spec* leds[] = {
    &LED0,
    &LED1,
    &LED2,
    &LED3
};

void configure_gpio(const struct gpio_dt_spec* LED_ptr)
{
    if (!gpio_is_ready_dt(LED_ptr))
    {
        exit(-1);
    }
    int ret = gpio_pin_configure_dt(LED_ptr, GPIO_OUTPUT_INACTIVE);
    if (ret < 0)
    {
        exit(ret);
    }
}

int main(void)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        configure_gpio(leds[i]);
    }

    while (1)
    {   
        for (uint8_t i = 0; i < 4; i++)
        {
            k_msleep(500);
            gpio_pin_toggle_dt(leds[i]);
        }
        k_msleep(500);
        for (int8_t i = 3; i >= 0; i--)
        {
            k_msleep(500);
            gpio_pin_toggle_dt(leds[i]);
        }
        k_msleep(500);
    }

    return 0;
}