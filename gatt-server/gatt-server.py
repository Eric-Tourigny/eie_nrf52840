"""Connector from Bluetooth Classic Controller to BLE (Bluetooth Low Energy) GATT server with controller status

Based on pygame (for Bluetooth Classic controller) and bless (for BLE GATT server) modules and examples:
- https://github.com/pygame/pygame/blob/main/examples/joystick.py
- https://github.com/kevincar/bless
- https://github.com/kevincar/bless/blob/master/examples/gattserver_notify.py
"""

import pygame
import asyncio
import logging

from bless import BlessServer, GATTCharacteristicProperties, GATTAttributePermissions     # type: ignore

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(name=__name__)


def BT_UUID_128_ENCODE(a, b, c, d, e):
    """Convenience function to encode UUID in same manner as C Macro"""
    return f"{a:08x}-{b:04x}-{c:04x}-{d:04x}-{e:012x}"


# Define device name
SERVER_NAME = "EiE 5666 Test GATT Server"

# Define service and characteristic UUIDs
CONTROLLER_SERVICE_ID = BT_UUID_128_ENCODE(0x660ED089, 0xB702, 0x356C, 0x594D, 0x2A471437E5C7)
BUTTON_CHARACTERISTIC_ID = BT_UUID_128_ENCODE(0x03A57D8B, 0x8092, 0xF3E6, 0x5B9D, 0x2002757871D3 )

async def init_gatt_server():
    logger.debug("Creating GATT Server")
    server = BlessServer(name=SERVER_NAME, loop=asyncio.get_event_loop())

    gatt_tree = {
        CONTROLLER_SERVICE_ID : {
            BUTTON_CHARACTERISTIC_ID : {
                "Properties": (
                      GATTCharacteristicProperties.read
                    | GATTCharacteristicProperties.write
                    | GATTCharacteristicProperties.notify
                ),
                "Permissions": (
                      GATTAttributePermissions.readable
                    | GATTAttributePermissions.writeable
                )
            }
        }
    }

    logger.debug("Initializing GATT tree")
    await server.add_gatt(gatt_tree)

    logger.debug("Creating GATT server")
    await server.start()

    logger.debug("Advertising on BLE")
    
    return server

async def init_controller():
    """Find, initialize and return a pygame joystick"""

    logger.debug("Searching for controllers")
    pygame.init()

    while True:
        pygame.joystick.init()                              # Detect controllers

        if pygame.joystick.get_count() != 0:                # If any controllers are found
            controller = pygame.joystick.Joystick(0)        # Take the first controller
            controller.init()                               # Initialize it
            logger.debug("Controller Connected")
            return controller
        
        pygame.joystick.quit()                              # Clear controller data, so we can try again

        await asyncio.sleep(0)                              # Yield control

async def main():
    controller, server = await asyncio.gather(init_controller(), init_gatt_server())


if __name__ == "__main__":
    asyncio.run(main())