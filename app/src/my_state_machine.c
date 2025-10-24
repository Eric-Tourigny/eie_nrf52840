#include <zephyr/smf.h>

#include "BTN.h"
#include "LED.h"
#include "my_state_machine.h"

/*----------------------------------------------------------------------------------------------------------
 * Function Prototypes
 *--------------------------------------------------------------------------------------------------------*/

static void S1_enter(void* o);
static void S2_enter(void* o);
static void S3_enter(void* o);
static void S4_enter(void* o);

static enum smf_state_result S0_run(void* o);
static enum smf_state_result S1_run(void* o);
static enum smf_state_result S2_run(void* o);
static enum smf_state_result S3_run(void* o);
static enum smf_state_result S4_run(void* o);

static void clear_LEDs(void* o);

/*----------------------------------------------------------------------------------------------------------
 * Typedefs
 *--------------------------------------------------------------------------------------------------------*/

enum led_state_machine_states {
    S0,
    S1,
    S2,
    S3,
    S4
};

typedef struct {
    struct smf_ctx ctx;

    uint16_t count;
} led_state_object_t;


/*----------------------------------------------------------------------------------------------------------
 * Local Variables
 *--------------------------------------------------------------------------------------------------------*/

static const struct smf_state led_states[] = {
    [S0] = SMF_CREATE_STATE(NULL, S0_run, clear_LEDs, NULL, NULL),
    [S1] = SMF_CREATE_STATE(S1_enter, S1_run, clear_LEDs, NULL, NULL),
    [S2] = SMF_CREATE_STATE(S2_enter, S2_run, clear_LEDs, NULL, NULL),
    [S3] = SMF_CREATE_STATE(S3_enter, S3_run, clear_LEDs, NULL, NULL),
    [S4] = SMF_CREATE_STATE(S4_enter, S4_run, clear_LEDs, NULL, NULL)
};

static led_state_object_t led_state_object;

/*----------------------------------------------------------------------------------------------------------
 * Local Functions
 *--------------------------------------------------------------------------------------------------------*/

static enum smf_state_result S0_run()
{
    if (BTN_check_clear_pressed(BTN0))
    {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S1]);
    }
    return SMF_EVENT_HANDLED;
}

static void S1_enter(void* o)
{
    LED_blink(LED0, 4);
}

static enum smf_state_result S1_run()
{
    if (BTN_check_clear_pressed(BTN1))
    {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S2]);
    }
    if (BTN_check_clear_pressed(BTN2))
    {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S4]);
    }
    if (BTN_check_clear_pressed(BTN3))
    {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S0]);
    }
    return SMF_EVENT_HANDLED;
}



static void S2_enter(void* o)
{
    LED_set(LED0, 1);
    LED_set(LED2, 1);
}

static enum smf_state_result S2_run()
{
    if (BTN_check_clear_pressed(BTN3))
    {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S0]);
    }
    else if (led_state_object.count == 1000)
    {
            smf_set_state(SMF_CTX(&led_state_object), &led_states[S3]);
    }
    led_state_object.count++;
    return SMF_EVENT_HANDLED;
}

static void S3_enter(void* o)
{
    LED_set(LED1, 1);
    LED_set(LED3, 1);

}

static enum smf_state_result S3_run()
{
    if (BTN_check_clear_pressed(BTN3))
    {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S0]);
    }
    else if (led_state_object.count == 2000)
    {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S2]);
    }
    led_state_object.count++;
    return SMF_EVENT_HANDLED;
}

static void S4_enter(void* o)
{
    LED_blink(LED0, 16);
    LED_blink(LED1, 16);
    LED_blink(LED2, 16);
    LED_blink(LED3, 16);
}

static enum smf_state_result S4_run()
{
    if (BTN_check_clear_pressed(BTN3))
    {
        smf_set_state(SMF_CTX(&led_state_object), &led_states[S0]);
    }
    return SMF_EVENT_HANDLED;
}

static void clear_LEDs()
{
    LED_set(LED0, 0);
    LED_set(LED1, 0);
    LED_set(LED2, 0);
    LED_set(LED3, 0);
    led_state_object.count = 0;
}


/*----------------------------------------------------------------------------------------------------------
 * Global Functions
 *--------------------------------------------------------------------------------------------------------*/

void state_machine_init()
{
    led_state_object.count = 0;
    smf_set_initial(SMF_CTX(&led_state_object), &led_states[S0]);
}

int state_machine_run()
{
    return smf_run_state(SMF_CTX(&led_state_object));
}
