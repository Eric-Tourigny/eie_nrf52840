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


void configure_gpio(const struct gpio_dt_spec* LED_ptr)
{
    if (!gpio_is_ready_dt(LED_ptr))
    {
        exit(-1);
    }
    int ret = gpio_pin_configure_dt(LED_ptr, GPIO_OUTPUT_ACTIVE);
    if (ret < 0)
    {
        exit(ret);
    }
}

int main(void)
{
    configure_gpio(&led0);
    configure_gpio(&led1);
    configure_gpio(&led2);
    configure_gpio(&led3);
    

    while (1)
    {
        gpio_pin_toggle_dt(&led0);
        gpio_pin_toggle_dt(&led1);
        gpio_pin_toggle_dt(&led2);
        gpio_pin_toggle_dt(&led3);

        k_msleep(1000);
    }

    return 0;
}