import asyncio
from bleak import BleakClient

# address detected using a 'discover' example script
# MODEL_NUMBR_UUID created wwith arduino Code
address = "41:60:FD:08:97:F8"
MODEL_NUMBR_UUID = "19B10011-E8F2-537E-4F6C-D104768A1214"

# function to be run asynchronously with asyncio
async def run(address):
    async with BleakClient(address) as client:
        try:
            while True:
                model_number = await client.read_gatt_char(MODEL_NUMBR_UUID)
                out = int.from_bytes(model_number, byteorder = 'little', signed = False)
                if out == 1 :
                    print("\nO Detected\n")
                elif out == 2 :
                    print("\nLR swipe detected\n")
                elif out == 3 :
                    print("\nRL swipe detected\n")
                elif out == 4 :
                    print("\nJab detected\n")
                elif out == 5 :
                    print("\nSpin detected\n")
                asyncio.wait(500)
        except(KeyboardInterrupt):
            print("Keyboard Interupt detected, quitting program")

# Part of the code that actually runs
loop = asyncio.get_event_loop()
loop.run_until_complete(run(address))