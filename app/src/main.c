/*
 * main.c
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>
#include <zephyr/types.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1

/***************************************************************************************************************************************
 * Define Globals
 ***************************************************************************************************************************************/

static struct bt_uuid_128 ble_custom_service_id = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x00000000, 0x1111, 0x2222, 0x3333, 0x000000000000));
static struct bt_uuid_128 ble_custom_characteristic_id = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x00000000, 0x1111, 0x2222, 0x3333, 0x000000000001));
static struct bt_conn* ble_connection;


/***************************************************************************************************************************************
 * Declare Local Functions
 ***************************************************************************************************************************************/

static void ble_on_device_connected(struct bt_conn* conn, uint8_t err);
static void ble_on_device_disconnected(struct bt_conn* conn, uint8_t reason);


/***************************************************************************************************************************************
 * Configure BLE
 ***************************************************************************************************************************************/

BT_CONN_CB_DEFINE(conn_callbacks) = {
  .connected = ble_on_device_connected,
  .disconnected = ble_on_device_disconnected
};


/***************************************************************************************************************************************
 * Define Local Functions
 ***************************************************************************************************************************************/

void ble_on_device_connected(struct bt_conn* conn, uint8_t err) {
  if (err != 0) {
    bt_conn_unref(conn);
    ble_connection = NULL;
    return;
  }
  
  if (conn != ble_connection) {
    return;
  }

  char mac_address[BT_ADDR_LE_STR_LEN];
  bt_addr_le_to_str(bt_conn_get_dst(conn), mac_address, sizeof(mac_address));
  printk("Connected %s\n", mac_address);

}

void ble_on_device_disconnected(struct bt_conn* conn, uint8_t reason) {
  if (conn != ble_connection) {
    return;
  }

  bt_conn_unref(conn);
  ble_connection = NULL;

  kprint("Disconnected - %s", bt_hci_err_to_str(reason));
}

int main(void) {

  /*
  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }
  */

  while(1) {
    k_msleep(SLEEP_MS);
  }
	return 0;
}
