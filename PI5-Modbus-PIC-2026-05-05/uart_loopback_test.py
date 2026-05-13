#!/usr/bin/env python3
import serial
import time
import sys

"""
UART Loopback Test for Raspberry Pi 5
-------------------------------------
This script tests the UART TX/RX pins without an RS485 HAT.

Hardware Connection:
1. Identify the GPIO header on your Pi 5.
2. Connect Physical Pin 8 (GPIO 14 - TXD) to Physical Pin 10 (GPIO 15 - RXD) 
   using a jumper wire.

This script will send a message on TX and wait for it on RX.
"""

# Configuration (Matches your Modbus setup)
#PORT = '/dev/serial0'
PORT = '/dev/ttyAMA0'
BAUDRATE = 57600

def run_test():
    print(f"--- UART Loopback Test on {PORT} ---")
    print(f"Target: Physical Pin 8 (TX) -> Physical Pin 10 (RX)")
    
    try:
        # Initialize serial port
        ser = serial.Serial(
            port=PORT,
            baudrate=BAUDRATE,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=2
        )
        # Clear the buffer of any noise or leftover data
        ser.reset_input_buffer()
        ser.reset_output_buffer()

    except serial.SerialException as e:
        print(f"Error: Could not open serial port {PORT}: {e}")
        return

    test_message = b"KAI"
    
    #try:
    while True  :
            print(f"Sending: {test_message.decode()}")
            ser.write(test_message)
            ser.flush()  # Ensure all data is sent
            
            # Give it a tiny bit of time
            time.sleep(0.1)
            
            # Read back
            received = ser.read(len(test_message))
            
            if received == test_message:
                print(f"SUCCESS: Received matching data: {received.decode()}")
            elif len(received) == 0:
                print("FAILURE: No data received. Check your jumper wire connection.")
            else:
                print(f"FAILURE: Data mismatch.")
                print(f"  Sent: {test_message}")
                print(f"  Received: {received}")
            
    '''except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        ser.close()
        print("Test complete. Serial port closed.")'''

if __name__ == "__main__":
    run_test()
