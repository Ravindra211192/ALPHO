#!/usr/bin/env python3

import serial
import time

"""-----------------------------------------------------------------------------------------
Testing Modbus/RTU serial communication with PIC controller  for storing temperature values 
--------------------------------------------------------------------------------------------
"""

def receiver():

    try:
        # setting serial0 (/dev/serial0 -> /dev/ttyAMA10 on Pi 5) port with baudrate 57600, 
        # 8 data bits, no parity bit, 1 start and 1 stop bit. timeout = 3 seconds
        port = serial.Serial(port='/dev/serial0', baudrate = 57600, bytesize = 8, parity = serial.PARITY_NONE, stopbits = 1, 
                timeout = 3)
        
        # exception for port opening failure
    except serial.SerialException as e:
        print(f"Error: Could not open serial port: {e}")
        return

    print("About to receive from PIC controller ... ...")

    try:
        while True:
            rx = port.read()
            print(rx)
            time.sleep(0.5)

    except KeyboardInterrupt:
        print("\nProgram stopped by user (Ctrl+C).")

    finally:
        port.close()
        print("Serial port closed.")

'''
Main program starts here
'''
def main():

    print("Starting Modbus/RTU serial communication ... ...")
    receiver()

if __name__ == "__main__":
    main()