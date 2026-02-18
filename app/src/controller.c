/*
 * controller.c
 */

#include "controller.h"

#include <errno.h>
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


/***************************************************************************************************************************************
 * Defines
 ***************************************************************************************************************************************/

#define PERIPHERAL_NAME "EiE 5666 Test GATT Server"
#define MIN_RSSI -50


/***************************************************************************************************************************************
 * Declare Local Functions
 ***************************************************************************************************************************************/

/*
 * Callback function for when a BLE advertisement is received.
 * If the connection to the controller (`ble_connection`) is not already established, checks if the advertised name matches `PERIPHERAL_NAME`, and
 * the signal strength (rssi) is greater than `MIN_RSSI`, it creates a connection and stops scanning for connection requests.
 */
static void ble_on_advertisement_received(const bt_addr_le_t* addr, int8_t rssi, uint8_t adv_type, struct net_buf_simple* buf);

/*
 * Copies name into `user_data` buffer if advertisement chunk `data` contains a device name and returns whether a name was found.
 */
static bool ble_get_adv_device_name_cb(struct bt_data* data, void* user_data);

/*
* Callback for when BLE device connection.
* Ensures the connection `conn` matches the connection that was selected, `ble_connection`.
* Clears the connection information if an error occurred.
* Discovers controller BLE service and characteristics.
*/
static void ble_on_device_connected(struct bt_conn* conn, uint8_t err);

/*
 * Callback for BLE device is disconnection, clears connection information (`ble_connection`)
 */
static void ble_on_device_disconnected(struct bt_conn* conn, uint8_t reason);

/*
 * Callback to discover GATT characteristics and register notifications.
 * Checks if `attr` matches each of the expected characteristic UUIDs, in which case it sets `next_subscribe_params` sets next subscribe params to corresponding variable.
 * Whenever `attr` is a CCC descriptor, sets up notifications for `next_subscribe_params` (if any), thus enabling notifications for the last characteristic.
 * Continues GATT discovery (return BT_GATT_ITER_CONTINUE) until end of the tree (a NULL `attr`) is reached (return BT_GATT_ITER_STOP)
 * Expected to be used with bt_gatt_discover to scan the entire GATT tree.  
 */
static uint8_t gatt_characteristic_discover(struct bt_conn* conn, const struct bt_gatt_attr* attr, struct bt_gatt_discover_params* params);


static uint8_t button_update_func(struct bt_conn* conn, struct bt_gatt_subscribe_params* params, const void* data, uint16_t length);


/***************************************************************************************************************************************
 * Global Variables
 ***************************************************************************************************************************************/

// The UUIDs for services and characteristics
static const struct bt_uuid_128 BUTTON_CHARACTERISTIC_ID = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x03A57D8B, 0x8092, 0xF3E6, 0x5B9D, 0x2002757871D3));
static const struct bt_uuid_16 CCC_UUID = BT_UUID_INIT_16(BT_UUID_GATT_CCC_VAL);      // UUID for any GATT CCC notify attribute

static int err;                                                             // The last BLE error code received
static struct bt_conn* ble_connection = NULL;                               // The BLE connection to the controller
static struct bt_gatt_discover_params discover_params;                      // Parameters for asynchronous GATT characteristic discovery, must remain valid until discovery is complete

static struct bt_gatt_subscribe_params *next_subscribe_params = NULL;                                   // CCC subscription parameters for current characteristic, during discovery
static struct bt_gatt_subscribe_params button_subscribe_params = {.notify = button_update_func};        // CCC subscription parameters for button characteristic


/***************************************************************************************************************************************
 * Configure BLE
 ***************************************************************************************************************************************/

 // Register device connected and disconnected functions with BLE module
BT_CONN_CB_DEFINE(conn_callbacks) = {
  .connected = ble_on_device_connected,
  .disconnected = ble_on_device_disconnected
};


/***************************************************************************************************************************************
 * Define Local Functions
 ***************************************************************************************************************************************/

void ble_on_advertisement_received(const bt_addr_le_t* addr, int8_t rssi, uint8_t adv_type, struct net_buf_simple* buf) {
    if (ble_connection != NULL) {                                               // If a connection has already been or is being established
        printk("Advertisement ignored - BLE connection already established\n");
        return;                                                                         // Ignore the advertisement
    }
    
    char mac_address[BT_ADDR_LE_STR_LEN];                                       // Buffer to hold MAC address
    bt_addr_le_to_str(addr, mac_address, sizeof(mac_address));                  // Extract MAC address into buffer

    char name[32] = {'\0'};                                                     // Buffer to hold the device name                                                              
    bt_data_parse(buf, ble_get_adv_device_name_cb, name);                       // Extract device name from advertising data, if present

    printk("Received advertisement from %s (device name: %s)\n", mac_address, name);

    if (strcmp(name, PERIPHERAL_NAME) != 0) {                                   // If the device name doesn't match the set PERIPHERAL_NAME
        return;                                                                         // Ignore the advertisement
    }

    if (rssi < -50) {                                                           // If the connection strength is very weak
        printk("Connection strength too weak, ignoring advertisement (rssi=%d)", rssi);
        return;                                                                         // Ignore the advertisement
    }

    bt_le_scan_stop();                                                                                  // Stop scanning advertisements
    err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, BT_LE_CONN_PARAM_DEFAULT, &ble_connection);   // Create a connection, storing information in `ble_connection`
    if (err) {                                                                                          // If the connection failed
        printk("Error creating BLE connection - %s\n", bt_hci_err_to_str(err));
        ble_connection = NULL;                                                                          // Clear the connection information
        return;
    }

    printk("Connection Established\n");
}

bool ble_get_adv_device_name_cb(struct bt_data* data, void* user_data) {
    if (data->type == BT_DATA_NAME_COMPLETE || 
        data->type == BT_DATA_NAME_SHORTENED)                                   // If the data type is a name
    {
        memcpy(user_data, data->data, data->data_len);                               // Copy the name into `user_data`
        ((char*)user_data)[data->data_len] = '\0';                                   // Make the name a cstring by adding a null terminator
        return false;                                                                // Stop parsing
    }
    return true;                                                               // Continue parsing this advertising packet
}

void ble_on_device_connected(struct bt_conn* conn, uint8_t err) {
    if (conn == ble_connection) {                                               // If the connection is actually the one we tried to make (to the controller)
        
        if(err != 0) {                                                                  // If the connection failed
            bt_conn_unref(conn);                                                            // Clear the connection information
            ble_connection = NULL;                                                          // Indicate no connection is currently made
            return;
        }

        char mac_address[BT_ADDR_LE_STR_LEN];                                          // Define a buffer to hold the MAC address
        bt_addr_le_to_str(bt_conn_get_dst(conn), mac_address, sizeof(mac_address));     // Copy destination MAC address into buffer
        printk("BLE connected to %s", mac_address);

        discover_params.uuid = NULL;                                         // Search for any UUID
        discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;        // Search from first GATT attribute
        discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;           // To the last GATT attribute
        discover_params.func = gatt_characteristic_discover;                 // Process attributes with gatt_characteristic_discover
        discover_params.type = BT_GATT_DISCOVER_ATTRIBUTE;                   // Search for any attribute

        err = bt_gatt_discover(ble_connection, &discover_params);           // Setup GATT attributes
        if (err != 0) {
            printk("Service discovery failed (err - %u)", err);
            return;
        }
    }
}

void ble_on_device_disconnected(struct bt_conn* conn, uint8_t reason) {
    if (conn == ble_connection) {                                               // If the *controller* connection was lost
        bt_conn_unref(conn);                                                            // Clear connection information
        ble_connection = NULL;                                                          // Indicate no connection is currently made
        printk("BLE disconnected - %s", bt_hci_err_to_str(reason));
    }
}

uint8_t gatt_characteristic_discover(struct bt_conn* conn, const struct bt_gatt_attr* attr, struct bt_gatt_discover_params* params) {
    if (attr == NULL) {                                                             // If the last attribute has been reached
        return BT_GATT_ITER_STOP;                                                       // Stop searching attributes
    }
    
    if (bt_uuid_cmp(discover_params.uuid, &CCC_UUID.uuid)) {                     // If CCC descriptor
        if (next_subscribe_params != NULL) {                                    // If the previous characteristic set to be subscribed
            next_subscribe_params->value = BT_GATT_CCC_NOTIFY;                      // Configure as notifications
            next_subscribe_params->ccc_handle = attr->handle;                       // Set subscription handle (GATT index)
            err = bt_gatt_subscribe(conn, next_subscribe_params);                  // Subscribe to characteristic
            if (err != 0) {
                printk("GATT discovery failed\n");
            }
            next_subscribe_params = NULL;                                           // Indicate subscription is complete 
        }
    } else if (bt_uuid_cmp(discover_params.uuid, &BUTTON_CHARACTERISTIC_ID.uuid)) {      // If button characteristic ID
        next_subscribe_params = &button_subscribe_params;                               // Setup subscription, for when CCC found
    }

    discover_params.start_handle += 1;                                              // Move to the next attribute
    return BT_GATT_ITER_CONTINUE;                                                   // Continue discovering attributes
}


uint8_t init_bluetooth() {
    err = bt_enable(NULL);                                                      // Initialize bluetooth
    if (err) {
        printk("Bluetooth init failed (err = %s)\n", bt_hci_err_to_str(err));
        return err;
    } else {
        printk("Bluetooth initialized\n");
    }

    bt_le_scan_start(BT_LE_SCAN_ACTIVE, ble_on_advertisement_received);         // Start scanning for advertisements

    return 0;               // Indicate no error occurred
}

static uint8_t button_update_func(struct bt_conn* conn, struct bt_gatt_subscribe_params* params, const void* data, uint16_t length) {
    printk("Button Update Received (data: %u)", *((uint8_t*) data));
    return 0;               // Indicate no error occurred
}