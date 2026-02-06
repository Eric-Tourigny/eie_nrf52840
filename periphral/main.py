import asyncio
from bleak import BleakScanner, BleakClient

def BT_UUID_128_ENCODE(a, b, c, d, e):
    return f"{a:08x}-{b:04x}-{c:04x}-{d:04x}-{e:012x}"

DEVICE_NAME = "5666 EiE BLE Peripheral"
CHARACTERISTIC_READER = BT_UUID_128_ENCODE(0x0f23685f, 0xf0b3, 0xd1c5, 0xf407, 0xcca1454ca47a)
CHARACTERISTIC_WRITER = BT_UUID_128_ENCODE(0xb36dee99, 0xddf7, 0x9994, 0x2cb3, 0x518c3b0abd5e)

async def main():
    print("Discovering Devices...")
    devices = await BleakScanner.discover()

    target_device = None
    for d in devices: 
        print(d)
        if d.name == DEVICE_NAME:
            target_device = d

    if target_device is not None:
        async with BleakClient(target_device.address) as client:
            await client.write_gatt_char(CHARACTERISTIC_WRITER, b"hello", False)
            result = await client.read_gatt_char(CHARACTERISTIC_READER)
            print(result)

if __name__ == "__main__":
    asyncio.run(main())