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

/***************************************************************************************************************************************
 * Defines
 ***************************************************************************************************************************************/

#define SLEEP_MS 1
#define SERVER_NAME "EiE Central 5666"

/***************************************************************************************************************************************
 * Global Variables
 ***************************************************************************************************************************************/

static struct bt_uuid_128 ble_custom_service_id = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x00000000, 0x1111, 0x2222, 0x3333, 0x000000000000));
static struct bt_uuid_128 ble_custom_characteristic_id = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x00000000, 0x1111, 0x2222, 0x3333, 0x000000000001));
static struct bt_conn* ble_connection;


/***************************************************************************************************************************************
 * Declare Local Functions
 ***************************************************************************************************************************************/

static void ble_on_device_connected(struct bt_conn* conn, uint8_t err);
static void ble_on_device_disconnected(struct bt_conn* conn, uint8_t reason);
static void ble_on_advertisement_received(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type, struct net_buf_simple *buf);
static bool ble_get_adv_device_name_cb(struct bt_data* data, void* user_data);


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

void ble_on_advertisement_received(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type, struct net_buf_simple *buf) {
  if (ble_connection == NULL) {
    return;
  }
  if (adv_type != BT_GAP_ADV_TYPE_ADV_IND && adv_type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND) {
    return;
  }

  char name[32];  
  bt_data_parse(buf, ble_get_adv_device_name_cb, name);

  char mac_address[BT_ADDR_LE_STR_LEN];
  bt_addr_le_to_str(bt_conn_get_dst(conn), mac_address, sizeof(mac_address));
  printk("Received Advertisement: %s\n", mac_address);
  printk("Device name: %s", name);
  printk("Signal Strength %d", rssi);

  // Do not connect if the signal strength is poor
  if (rssi < -50) {
    return;         
  }

  // Do not connect to anything except the named server
  if (name != SERVER_NAME) {
    return;
  }

  // Create a connection to addr and store its data in ble_connection
  if (bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, BT_LE_CONN_PARAM_DEFAULT, &ble_connection) != 0) {
    printk("BLE connection creation error - %d", err);
    ble_connection = NULL;
  }
}

bool ble_get_adv_device_name_cb(struct bt_data* data, void* name) {
  if (data->type == BT_DATA_NAME_COMPLETE || data->type == BT_DATA_NAME_SHORTENED) {
    memcpy(data->data, name, data->data_len)
    name[data->data_len] = '\0';
    return false;                   // End parsing
  }
  return true;                      // Continue parsing
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

  int err;

  err = bt_enable(NULL);
  if (err != 0) {
    printk("BLE Initialization Error - %d", err);
    return;
  } else {
    printk("Bluetooth Initialized");
  }

  err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, ble_on_advertisement_received);
  if (err != 0) {
    printk("BLE Scan Start Error - %d");
    return;
  }

  k_sleep(K_FOREVER);
}
