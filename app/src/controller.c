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

/*
 * Callback for notifications of button updates. Expects a single byte of `data` representing a bitmask for whether each button is on.
 * Updates `last_button_states` and `button_events` so button API functions can receive appropriate updates.
 */
static uint8_t button_update_func(struct bt_conn* conn, struct bt_gatt_subscribe_params* params, const void* data, uint16_t length);

/*
 * Callback for notifications of joystick updates
 */
static uint8_t joystick_update_func(struct bt_conn* conn, struct bt_gatt_subscribe_params* params, const void* data, uint16_t length);

/***************************************************************************************************************************************
 * Global Variables
 ***************************************************************************************************************************************/

// The UUIDs for services and characteristics
static const struct bt_uuid_128 BUTTON_CHARACTERISTIC_ID = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x03A57D8B, 0x8092, 0xF3E6, 0x5B9D, 0x2002757871D3));
static const struct bt_uuid_128 JOYSTICK_CHARACTERISTIC_ID = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x08154e46, 0x99b7, 0x5ea3, 0xd6d7, 0xc983e5568d64));
static const struct bt_uuid_16 CCC_UUID = BT_UUID_INIT_16(BT_UUID_GATT_CCC_VAL);      // UUID for any GATT CCC notify attribute

static int err;                                                             // The last BLE error code received
static struct bt_conn* ble_connection = NULL;                               // The BLE connection to the controller
static struct bt_gatt_discover_params discover_params;                      // Parameters for asynchronous GATT characteristic discovery, must remain valid until discovery is complete

static struct bt_gatt_subscribe_params *next_subscribe_params = NULL;                                   // CCC subscription parameters for current characteristic, during discovery
static struct bt_gatt_subscribe_params button_subscribe_params = {.notify = button_update_func};        // CCC subscription parameters for button characteristic
static struct bt_gatt_subscribe_params joystick_subscribe_params = {.notify = joystick_update_func};    // CCC subscription parameters for joystick characteristic

static uint8_t last_button_states = 0;          // Bitmask representing last state of each button
static uint8_t button_events = 0;               // Bitmask representing whether event occurred on each button
joystick_t joystick_state;                      // Global variable for current state of joystick

/***************************************************************************************************************************************
 * Configure BLE
 ***************************************************************************************************************************************/

 // Register device connected and disconnected functions with BLE module
BT_CONN_CB_DEFINE(conn_callbacks) = {
  .connected = ble_on_device_connected,
  .disconnected = ble_on_device_disconnected
};


/***************************************************************************************************************************************
 * BLE Local Function Definitions
 ***************************************************************************************************************************************/

static void ble_on_advertisement_received(const bt_addr_le_t* addr, int8_t rssi, uint8_t adv_type, struct net_buf_simple* buf) {
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

static bool ble_get_adv_device_name_cb(struct bt_data* data, void* user_data) {
    if (data->type == BT_DATA_NAME_COMPLETE || 
        data->type == BT_DATA_NAME_SHORTENED)                                   // If the data type is a name
    {
        memcpy(user_data, data->data, data->data_len);                               // Copy the name into `user_data`
        ((char*)user_data)[data->data_len] = '\0';                                   // Make the name a cstring by adding a null terminator
        return false;                                                                // Stop parsing
    }
    return true;                                                               // Continue parsing this advertising packet
}

static void ble_on_device_connected(struct bt_conn* conn, uint8_t err) {
    if (conn == ble_connection) {                                               // If the connection is actually the one we tried to make (to the controller)
        
        if(err != 0) {                                                                  // If the connection failed
            bt_conn_unref(conn);                                                            // Clear the connection information
            ble_connection = NULL;                                                          // Indicate no connection is currently made
            return;
        }

        char mac_address[BT_ADDR_LE_STR_LEN];                                          // Define a buffer to hold the MAC address
        bt_addr_le_to_str(bt_conn_get_dst(conn), mac_address, sizeof(mac_address));     // Copy destination MAC address into buffer
        printk("BLE connected to %s\n", mac_address);

        discover_params.uuid = NULL;                                         // Search for any UUID
        discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;        // Search from first GATT attribute
        discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;           // To the last GATT attribute
        discover_params.func = gatt_characteristic_discover;                 // Process attributes with gatt_characteristic_discover
        discover_params.type = BT_GATT_DISCOVER_ATTRIBUTE;                   // Search for any attribute

        err = bt_gatt_discover(ble_connection, &discover_params);           // Setup GATT attributes
        if (err != 0) {
            printk("Service discovery queueing failed (err - %u)\n", err);
            return;
        }
        printk("Service discovery queueing successful\n");
    }
}

static void ble_on_device_disconnected(struct bt_conn* conn, uint8_t reason) {
    if (conn == ble_connection) {                                               // If the *controller* connection was lost
        bt_conn_unref(conn);                                                            // Clear connection information
        ble_connection = NULL;                                                          // Indicate no connection is currently made
        printk("BLE disconnected - %s", bt_hci_err_to_str(reason));
    }
}

static uint8_t gatt_characteristic_discover(struct bt_conn* conn, const struct bt_gatt_attr* attr, struct bt_gatt_discover_params* params) {
    if (attr == NULL) {                                                             // If the last attribute has been reached
        printk("Reached final characteristic\n");
        return BT_GATT_ITER_STOP;                                                       // Stop searching attributes
    }

    char uuid_buffer[64] = {0};
    bt_uuid_to_str(attr->uuid, uuid_buffer, 64);
    printk("Discovering characteristic (handle - %d, uuid - %s)\n", attr->handle, uuid_buffer);
    
    if (0 == bt_uuid_cmp(attr->uuid, &CCC_UUID.uuid)) {                     // If CCC descriptor
        if (next_subscribe_params != NULL) {                                    // If the previous characteristic set to be subscribed
            next_subscribe_params->value = BT_GATT_CCC_NOTIFY;                      // Configure as notifications
            next_subscribe_params->ccc_handle = attr->handle;                       // Set subscription handle (GATT index)

            err = bt_gatt_subscribe(conn, next_subscribe_params);                  // Subscribe to characteristic
            if (err != 0) {
                printk("GATT notify subscription failed\n");
            }
            next_subscribe_params = NULL;                                           // Indicate subscription is complete 
            printk("Subscription to characteristic successful\n");
        }
    } else if (0 == bt_uuid_cmp(attr->uuid, &BUTTON_CHARACTERISTIC_ID.uuid)) {      // If matches button characteristic
        printk("Button characteristic discovered\n");
        next_subscribe_params = &button_subscribe_params;                               // Setup subscription, for when CCC found
        next_subscribe_params->value_handle = attr->handle;                                 // Set handle to retrieve / set this attribute's value
    } else if (0 == bt_uuid_cmp(attr->uuid, &JOYSTICK_CHARACTERISTIC_ID.uuid)) {    // If matches joystick characteristic
        printk("Joystick characteristic discovered\n");
        next_subscribe_params = &joystick_subscribe_params;                             // Setup subscription, for when CCC found
        next_subscribe_params->value_handle = attr->handle;                                 // Set handle to retrieve / set this attribute's value
    }

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

    err = bt_le_scan_start(BT_LE_SCAN_ACTIVE, ble_on_advertisement_received);         // Start scanning for advertisements

    return err;               // Indicate no error occurred
}


/***************************************************************************************************************************************
 * Notification Callback Function Definitions
 ***************************************************************************************************************************************/

static uint8_t button_update_func(struct bt_conn* conn, struct bt_gatt_subscribe_params* params, const void* data, uint16_t length) {
    if (data == NULL) {                                                     // No data call indicates notification cancelled by GATT server
        printk("Button Characteristic Unsubscribed\n");
        return BT_GATT_ITER_STOP;                                               // Stop looking for notifications
    }

    uint8_t new_button_states = *((uint8_t*) data);                         // Extract boolean values for whether each button was pressed from data
    button_events |= last_button_states ^ new_button_states;                // Register new button events for each value that changed
    last_button_states = new_button_states;                                 // Update stored button states

    return BT_GATT_ITER_CONTINUE;                                           // Continue receiving button notifications
}

static uint8_t joystick_update_func(struct bt_conn* conn, struct bt_gatt_subscribe_params* params, const void* data, uint16_t length) {
    if (data == NULL) {                                                     // No data call indicates notification cancelled by GATT server
        printk("Joystick Characteristic Unsubscribed\n");
        return BT_GATT_ITER_STOP;                                               // Stop looking for notifications
    }

    // Update current joystick state - each byte should be either 0, 1 or 2
    joystick_state.h = ((uint8_t*) data)[0];
    joystick_state.v = ((uint8_t*) data)[1];

    return BT_GATT_ITER_CONTINUE;                                           // Continue receiving joystick notifications
}


/***************************************************************************************************************************************
 * Global API Function Definitions
 ***************************************************************************************************************************************/

bool button_check_clear_pressed(uint8_t button_id) {
    uint8_t button_index_mask = 1 << button_id;                             // Mask for bit where button's information is
    if ((button_events & button_index_mask) &&                              // If a button event occurred at that index
        (last_button_states & button_index_mask))                           // And that button is currently down (1)
    {                         
        button_events &= ~button_index_mask;                                    // Clear the button event
        return true;                                                            // Return that the button was pressed
    }
    return false;                                                           // Return that the button was *not* pressed
}

bool button_check_clear_released(uint8_t button_id) {
    uint8_t button_index_mask = 1 << button_id;                             // Mask for bit where button's information is
    if ((button_events & button_index_mask) &&                              // If a button event occurred at that index
        !(last_button_states & button_index_mask))                          // And the button is currently up (0)
    {
        button_events &= ~button_index_mask;                                    // Clear the button event
        return true;                                                            // Return that the button was released
    }
    return false;                                                           // Return that the button was *not* released
}

bool button_check_held(uint8_t button_id) {
    uint8_t button_index_mask = 1 << button_id;                             // Mask for bit where button's information is
    return (last_button_states & button_index_mask) != 0;                  // Return whether button is currently down (1)
}
