"""
EiE Test GATT Server, based on the bless GATT server examples:
https://github.com/kevincar/bless/blob/master/examples/server.py
https://github.com/kevincar/bless/blob/master/examples/gattserver.py
"""

import logging
import asyncio
import itertools
import time

from typing import Any

from bless import BlessServer, BlessGATTCharacteristic, GATTCharacteristicProperties, GATTAttributePermissions   # type: ignore


def BT_UUID_128_ENCODE(a, b, c, d, e):
    """Convenience function to encode UUID in same manner as C Macro"""
    return f"{a:08x}-{b:04x}-{c:04x}-{d:04x}-{e:012x}"

# Define device name
SERVER_NAME = "EiE 5666 Test GATT Server"

# Define service and characteristic UUIDs
BLE_CUSTOM_SERVICE_ID = BT_UUID_128_ENCODE(0x00000000, 0x1111, 0x2222, 0x3333, 0x000000000000)
BLE_CUSTOM_CHARACTERISTIC_ID = BT_UUID_128_ENCODE(0x00000000, 0x1111, 0x2222, 0x3333, 0x000000000001)

# Configure Logging
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(name=__name__)


# Characteristic read callback function
def read_request(characteristic: BlessGATTCharacteristic, **kwargs) -> bytearray:
    logger.debug(f"Reading {characteristic.value}")
    return characteristic.value

# Characteristic write callback function
def write_request(characteristic: BlessGATTCharacteristic, value: Any, **kwargs):
    characteristic.value = value
    logger.debug(f"Char value set to {characteristic.value}")


async def run(loop):
    server = BlessServer(name=SERVER_NAME, loop=loop)   # Instantiate the server
    server.read_request_func = read_request                             # Callback function to handle read request on any characteristic
    server.write_request_func = write_request                           # Callback function to handle write request on any characteristic

    # Define tree of GATT services and characteristics
    gatt_tree = {
        BLE_CUSTOM_SERVICE_ID: {
            BLE_CUSTOM_CHARACTERISTIC_ID: {
                "Properties": (
                      GATTCharacteristicProperties.read
                    | GATTCharacteristicProperties.write
                    | GATTCharacteristicProperties.notify
                ),
                "Permissions": (
                      GATTAttributePermissions.readable
                    | GATTAttributePermissions.writeable
                ),
                "Value": None
            }
        }
    }
    await server.add_gatt(gatt_tree)

    logger.debug("Starting Server")
    await server.start()
    logger.debug("Advertising")

    return server

async def update(server: BlessServer):
    characteristic = server.get_characteristic(BLE_CUSTOM_CHARACTERISTIC_ID)
    if characteristic is None:
        logger.debug("Characteristic not found")
        return
    for i in itertools.count():
        time.sleep(1)
        characteristic.value = bytearray(f"Hello World - {i}".encode())
        server.update_value(BLE_CUSTOM_SERVICE_ID, BLE_CUSTOM_CHARACTERISTIC_ID)


async def main():
    loop = asyncio.get_event_loop()
    try:
        server = await run(loop)
        await update(server)
    except asyncio.CancelledError:
        logger.debug("Stopping Server")
        await server.stop()

if __name__ == "__main__":
    asyncio.run(main())
