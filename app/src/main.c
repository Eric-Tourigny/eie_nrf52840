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
#define PERIPHERAL_NAME "DESKTOP-9ENFI6B"

/***************************************************************************************************************************************
 * Global Variables
 ***************************************************************************************************************************************/

static struct bt_uuid_128 ble_custom_service_id = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x00000000, 0x1111, 0x2222, 0x3333, 0x000000000000LL));
static struct bt_uuid_128 ble_custom_characteristic_id = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x00000000, 0x1111, 0x2222, 0x3333, 0x000000000001LL));
static struct bt_conn* ble_connection = NULL;

static struct bt_uuid_16 discover_uuid = BT_UUID_INIT_16(BT_UUID_GATT_CCC_VAL);
static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params;


/***************************************************************************************************************************************
 * Declare Local Functions
 ***************************************************************************************************************************************/

static void ble_on_device_connected(struct bt_conn* conn, uint8_t err);
static void ble_on_device_disconnected(struct bt_conn* conn, uint8_t reason);
static void ble_on_advertisement_received(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type, struct net_buf_simple *buf);
static bool ble_get_adv_device_name_cb(struct bt_data* data, void* user_data);
static uint8_t discover_func(struct bt_conn* conn, const struct bt_gatt_attr* attr, struct bt_gatt_discover_params* params);
static uint8_t notify_func(struct bt_conn* conn, struct bt_gatt_subscribe_params* params, const void* data, uint16_t length);


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

// Callback for a connection being established
void ble_on_device_connected(struct bt_conn* conn, uint8_t err) {
  // If an error occurred, clear the global connection objects
  if (err != 0) {
    bt_conn_unref(conn);
    ble_connection = NULL;
    return;
  }
  
  // Ensure the connection matches the one we tried to make
  if (conn != ble_connection) {
    return;
  }

  // Define buffer to hold MAC address and copy the data into the buffer
  char mac_address[BT_ADDR_LE_STR_LEN];
  bt_addr_le_to_str(bt_conn_get_dst(conn), mac_address, sizeof(mac_address));

  // Send connection successful debug message
  printk("Connected %s\n", mac_address);

  discover_params.uuid = &ble_custom_service_id.uuid;              // Search for custom service
  discover_params.func = discover_func;                           // Callback 
  discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;   // Discover attributes starting from first attribute 
  discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;        // up until the last attribute
  discover_params.type = BT_GATT_DISCOVER_PRIMARY;                // Discover primary services

  err = bt_gatt_discover(ble_connection, &discover_params);
  if (err != 0) {
    printk("Service discovery failed (err - %d)\n", err);
    return;
  }

}

// Callback for the connection failing
void ble_on_device_disconnected(struct bt_conn* conn, uint8_t reason) {
  if (conn != ble_connection) {
    return;
  }

  bt_conn_unref(conn);
  ble_connection = NULL;

  printk("Disconnected - %s", bt_hci_err_to_str(reason));
}

// Callback for when an advertisement package is received
void ble_on_advertisement_received(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type, struct net_buf_simple *buf) {
  printk("Received Advertisement: ");

  if (ble_connection != NULL) {
    printk("BLE connection already established.\n");
    return;
  }
  /*  
  if (adv_type != BT_GAP_ADV_TYPE_ADV_IND && adv_type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND) {
    printk("Invalid advertisement type\n");
    return;
  }
  */

  char mac_address[BT_ADDR_LE_STR_LEN];
  bt_addr_le_to_str(addr, mac_address, sizeof(mac_address));
  printk("Received Advertisement: %s\n", mac_address);

  char name[32] = {0};  
  bt_data_parse(buf, ble_get_adv_device_name_cb, name);

  // Do not connect to anything except the named server
  if (strcmp(name, PERIPHERAL_NAME) != 0) {
    printk("Invalid Device Name - %s\n", name);
    return;
  } else {
    printk("Correct Device Name - %s\n", name);
  }

  // Do not connect if the signal strength is poor
  if (rssi < -50) {
    printk("Signal Strength too Weak (rsi %d)\n", rssi);
    return;
  } else {
    printk("Signal Strength: %d\n", rssi);
  }

  int err = bt_le_scan_stop();
  if (err != 0) {
    printk("Error stoping BLE scan - %d\n", err);
    return;
  }

  // Create a connection to addr and store its data in ble_connection
  err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, BT_LE_CONN_PARAM_DEFAULT, &ble_connection);
  if (err != 0) {
    printk("BLE connection creation error - %d\n", err);
    ble_connection = NULL;
    return;
  }

  printk("Connection Established\n");
}

// Callback to extract device name from bt_data
bool ble_get_adv_device_name_cb(struct bt_data *data, void *user_data) {
  char *name = user_data;
  if (data->type == BT_DATA_NAME_COMPLETE || data->type == BT_DATA_NAME_SHORTENED) {
    memcpy(name, data->data, data->data_len);
    name[data->data_len] = '\0';
    return false;                   // End parsing
  }
  return true;                      // Continue parsing
}

uint8_t discover_func(struct bt_conn* conn, const struct bt_gatt_attr* attr, struct bt_gatt_discover_params* params) {
  int err;

  // If all characteristics have been examined
  if (attr == NULL) {
    printk("Discover complete\n");        // Send discovery complete message
    memset(params, 0, sizeof(*params));   // Clear params
    return BT_GATT_ITER_STOP;             // End iteration
  }

  printk("[ATTRIBUTE] handle %u\n", attr->handle);    // Print attribute number

  if (bt_uuid_cmp(discover_params.uuid, &ble_custom_service_id.uuid) == 0) {   // The attribute is the custom service
    printk("Custom service discovered\n");

    discover_params.uuid = &ble_custom_characteristic_id.uuid;              // Search for custom characteristic
    discover_params.start_handle = attr->handle + 1;                        // Start from just after custom service
    discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;                 // Search for a characteristic

    err = bt_gatt_discover(conn, &discover_params);                         // Search for the characteristic
    if (err != 0) {
      printk("Discover failed - err %d", err);
    }
  } else if (bt_uuid_cmp(discover_params.uuid, &ble_custom_characteristic_id.uuid) == 0) {  // The attribute is the custom characteristic
    printk("Custom characteristic discovered\n");

    discover_params.uuid = &discover_uuid.uuid;                             // UUID is empty (all zeros)
    discover_params.start_handle = attr->handle + 2;                        // Start after characteristic declaration and value
    discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;                      // Search for descriptor (CCCD)
    subscribe_params.value_handle = bt_gatt_attr_value_handle(attr);        // Set handle to retrieve / set this attribute's value

    err = bt_gatt_discover(conn, &discover_params);                         // Search for the attribute
    if (err != 0) {
      printk("Discover failed - err %d", err);
    }
  } else {                                                                  // Attribute is CCC descriptor
    printk("CCC Descriptor Discovered\n");

    subscribe_params.notify = notify_func;                                  // Set callback for notifications from custom characteristic
    subscribe_params.value = BT_GATT_CCC_NOTIFY;                            // Enable notifications (not indications)
    subscribe_params.ccc_handle = attr->handle;                             // Note handle for ccc attribute
  
    err = bt_gatt_subscribe(conn, &subscribe_params);                       // Subscribe to notifications
    if (err != 0 && err != -EALREADY) {                                     // If any error other than operation already in progress occurs
      printk("Subscribe failed - err %d", err);
    } else {
      printk("[SUBSCRIBED]\n");
    }
  }
  return BT_GATT_ITER_STOP;                                                 // Stop iteration through attributes
}

// Callback for notifications on custom characteristic
uint8_t notify_func(struct bt_conn* conn, struct bt_gatt_subscribe_params* params, const void* data, uint16_t length) {

  if (data == NULL) {
    printk("[UNSUBSCRIBED]\n");
    params->value_handle = 0U;
    return BT_GATT_ITER_STOP;     // End notifications
  }

  uint8_t* value = (uint8_t*) data;

  printk("[NOTIFICATION] data %p length %u\n", data, length);
  for (int i = 0; i < MIN(length, 16); i++) {                   // For up to first 16 bytes
    printk("%c", value[i]);                                     // Send corresponding ASCII character
  }
  
  return BT_GATT_ITER_CONTINUE;   // Continue receiving notifications

}


// Main function
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
    return err;
  } else {
    printk("Bluetooth Initialized\n");
  }

  err = bt_le_scan_start(BT_LE_SCAN_ACTIVE_CONTINUOUS, ble_on_advertisement_received);
  if (err != 0) {
    printk("BLE Scan Start Error - %d", err);
    return err;
  }

  k_sleep(K_FOREVER);
}
