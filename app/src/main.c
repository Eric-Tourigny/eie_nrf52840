/*
 * main.c
 */


#include "BTN.h"
#include "LED.h"

#include <stdio.h>
#include <string.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/printk.h>


#define SLEEP_MS 1


/******************************************************************************************************************************************************
 * Define the UUIDs
 ******************************************************************************************************************************************************/

static const struct bt_uuid_128 ble_custom_service_uuid = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0xa3800744, 0x579b, 0xfdab, 0x6b41, 0xcf689a763abf));
static const struct bt_uuid_128 ble_custom_characteristic_uuid = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x0f23685f, 0xf0b3, 0xd1c5, 0xf407, 0xcca1454ca47a));


/******************************************************************************************************************************************************
 * Define the advertising data
 ******************************************************************************************************************************************************/

static const struct bt_data ble_advertising_data[] = {
  BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
  BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME))
};


/******************************************************************************************************************************************************
 * Define the characteristics
 ******************************************************************************************************************************************************/

#define BLE_CUSTOM_CHARACTERISTIC_MAX_DATA_LENGTH 20                                                                  // Size of the custom characteristic data
static uint8_t ble_custom_characteristic_user_data[BLE_CUSTOM_CHARACTERISTIC_MAX_DATA_LENGTH] = {};                   // Initial contents of custom characteristic
static ssize_t ble_custom_characteristic_simple_read_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset);
static ssize_t ble_custom_characteristic_simple_write_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr, const void* buf, uint16_t len, uint16_t offset, uint8_t flags);

/******************************************************************************************************************************************************
 * Construct the GATT Structure
 ******************************************************************************************************************************************************/

BT_GATT_SERVICE_DEFINE(
  ble_custom_service,                                   // The name of the variable storing the service's data
  BT_GATT_PRIMARY_SERVICE(&ble_custom_service_uuid),    // The UID for the service
  BT_GATT_CHARACTERISTIC (                              // The first characteristic in the service
    &ble_custom_characteristic_uuid.uuid,                     // The characteristic's UID
    BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,                   // Allow the characteristic to be read and written
    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,                   // Allow connecting devices to read and write the characteristic
    ble_custom_characteristic_simple_read_cb,                 // Function to call on characteristic read
    ble_custom_characteristic_simple_write_cb,                // Function to call on characteristic write
    ble_custom_characteristic_user_data                       // The initial data stored in the characteristic
  )
);


/******************************************************************************************************************************************************
 * Attribute Callback Definitions
 ******************************************************************************************************************************************************/

// Callback for when the custom characteristic is read from
static ssize_t ble_custom_characteristic_simple_read_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset) {
  const char* value = attr->user_data;                                            // Generate the characteristic value as the current written value
  return bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(value));   // Return the characteristic value
}

// Callback for when the custom characteristic is written
static ssize_t ble_custom_characteristic_simple_write_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr, const void* buf, uint16_t len, uint16_t offset, uint8_t flags) {
  if (len + offset > BLE_CUSTOM_CHARACTERISTIC_MAX_DATA_LENGTH)       // Check if the data to write can fit in the attribute
  {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);                    // If it is too large, return an error
  }

  memcpy(attr->user_data, buf, len);                                  // Copy the data into the attribute
  return len;                                                         // Return the length written
}


/******************************************************************************************************************************************************
 * Main Code
 ******************************************************************************************************************************************************/

int main(void) {
  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  while(1) {
    k_msleep(SLEEP_MS);
  }
	return 0;
}
