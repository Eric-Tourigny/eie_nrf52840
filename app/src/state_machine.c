#include <zephyr/smf.h>

#include "LED.h"
#include "BTN.h"
#include "printk.h"
#include "state_machine.h"

/*********************************************************************************************
 * Typedefs
 ********************************************************************************************/

#define STRING_SIZE 100

typedef enum {
    ENTER_BYTE = 0,
    ENTER_CODE,
    SEND,
    STANDBY
} state_t;

typedef struct {
    struct smf_ctx ctx;
    uint8_t ascii_code;
    uint8_t size;
    uint8_t string;
} state_object_t;

/*********************************************************************************************
 * Globals
 ********************************************************************************************/

state_object_t state_object;

/*********************************************************************************************
 * Function Prototypes
 ********************************************************************************************/

static void enter_enter_byte();
static enum smf_state_result run_enter_byte();
static void enter_enter_code();
static enum smf_state_result run_enter_code();
static void enter_send();
static enum smf_state_result run_send();
static void enter_standby();
static enum smf_state_result run_standby();


const struct smf_state states[] = {
    [ENTER_BYTE] =  SMF_CREATE_STATE(enter_enter_byte, run_enter_byte, NULL, NULL, NULL),
    [ENTER_CODE] =  SMF_CREATE_STATE(enter_enter_code, run_enter_code, NULL, NULL, NULL),
    [SEND] =        SMF_CREATE_STATE(enter_send, run_send, NULL, NULL),
    [STANDBY] =     SMF_CREATE_STATE(enter_standby, run_standby, NULL, NULL, NULL),
};


/*********************************************************************************************
 * State Function Definitions
 ********************************************************************************************/
static void enter_enter_byte()
{
    state_object.ascii_code = 0;
    LED_blink(LED3, LED_1HZ);
    return SMF_EVENT_HANDLED;
}

static enum smf_state_result run_enter_byte()
{
    if (BTN_check_clear_pressed(BTN0))
    {
        state_object.ascii_code <<= 1;
    }
    if (BTN_check_clear_pressed(BTN1))
    {
        state_object.ascii_code <<= 1;
        state_object.ascii_code |= 1;
    }
    if (BTN_check_clear_pressed(BTN2))
    {
        state_object.ascii_code = 0;
    }
    if (BTN_check_clear_pressed(BTN3))
    {
        state_object.string[state_object.size] = state_object.ascii_code;
        state_object.size++;
        smf_set_state(SMF_CTX(&state_object), &states[ENTER_CODE]);
    }
    return SMF_EVENT_HANDLED;
}

static void enter_enter_code()
{
    LED_blink(LED0, LED_4HZ);
}

static enum smf_state_result run_enter_code()
{
    if (BTN_check_clear_pressed(BTN0))
    {
        state_object.ascii_code = 0;
        smf_set_state(SMF_CTX(&state_object), &states[ENTER_BYTE]);
    }
    if (BTN_check_clear_pressed(BTN1))
    {
        state_object.ascii_code = 1;
        smf_set_state(SMF_CTX(&state_object), &states[ENTER_BYTE]);
    }
    if (BTN_check_clear_pressed(BTN2))
    {
        state_object.size = 0;
        state_object.ascii_code = 0;
        smf_set_state(SMF_CTX(&state_object), &states[ENTER_BYTE]);
    }
    if (BTN_check_clear_pressed(BTN3))
    {
        smf_set_state(SMF_CTX(&state_object), &states[SEND]);
    }
    return SMF_EVENT_HANDLED;
}

static void enter_send()
{
    LED_blink(LED3, LED_16HZ);
}

static enum smf_state_result run_send()
{
    if (BTN_check_clear_pressed(BTN2))
    {
        state_object.size = 0;
        smf_set_state(SMF_CTX(&state_object), &states[ENTER_BYTE]);
    }
    if (BTN_check_clear_pressed(BTN3))
    {
        state_object.string[state_object.size] = '\0';
        printk("%s", state_object.string);
    }
    return SMF_EVENT_HANDLED;
}

static void enter_standby()
{
    LED_pwm(LED0, 50);
    LED_pwm(LED0, 100);
    return SMF_EVENT_HANDLED;
}

static enum smf_state_result run_standby()
{
    if (BTN_check_clear_pressed(BTN0) || BTN_check_clear_pressed(BTN1) || BTN_check_clear_pressed(BTN2) || BTN_check_clear_pressed(BTN3))
    {
        smf_set_state(SMF_CTX(&state_object), state_object.ctx.previous);
    }
    return SMF_EVENT_HANDLED;
}

/*********************************************************************************************
 * Global Function Definitions
 ********************************************************************************************/

void init_state_machine()
{
    state_object.size = 0;

}

int run_state_machine()
{
    return 0;
}