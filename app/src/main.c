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

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

const struct gpio_dt_spec* leds[] = {
    &led0,
    &led1,
    &led2,
    &led3
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
        configure_gpio(leds[0]);
    }

    while (1)
    {   
        for (uint8_t i = 0; i < 4; i++)
        {
            k_msleep(500);
            gpio_pin_toggle_dt(leds[i]);
        }
        k_msleep(500);
        for (uint8_t i = 0; i < 4; i++)
        {
            gpio_pin_toggle_dt(leds[i]);
        }
    }

    return 0;
}