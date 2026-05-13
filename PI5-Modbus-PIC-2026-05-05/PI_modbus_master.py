'''#!/usr/bin/env python3'''


"""-----------------------------------------------------------------------------------------
PI5 Modbus/RTU Master - Reading data registers from PIC MF40 controller
via RS-485 HAT (RB-RS485) on Raspberry Pi 5

PIC MF40 Register Map (Holding Registers, Function Code 3):
---------------------------------------------------------------------------
RW Registers (address 0-23, each entry = 2 registers = 4 bytes = float32):
  Addr 0,1   : SCADA_Cmd0 (CMD32)
  Addr 2,3   : P_SET      (Power Setpoint)
  Addr 4,5   : TEMP_SET   (Temperature Setpoint)
  Addr 6,7   : TIMER_SET  (Timer Setpoint)
  Addr 8,9   : PID_Kp
  Addr 10,11 : PID_Ki
  Addr 12,13 : PID_Kd
  Addr 14,15 : GenOffUse   (2 bytes, uint16 stored as float)
  Addr 16,17 : StopAtTemp  (2 bytes, uint16 stored as float)
  Addr 18,19 : TimerDelay  (2 bytes, uint16 stored as float)
  Addr 20,21 : Spare
  Addr 22,23 : Spare

RO Registers (address 200-215, each entry = 2 registers = 4 bytes = float32):
  Addr 200,201 : SCADA_Status0 / STATUS32
  Addr 202,203 : P       (Actual Power)
  Addr 204,205 : TEMP    (Actual Temperature)
  Addr 206,207 : TIMER   (Actual Timer)
  Addr 208,209 : I       (Actual Current)
  Addr 210,211 : F       (Actual Frequency)
  Addr 212,213 : COS_PHI (Actual Cos-Phi)
  Addr 214,215 : REG%    (Regulator %)

Note: Addresses 1000+ enable little-endian byte order on PIC side.
--------------------------------------------------------------------------------------------
"""

import serial
import struct
import time
from pymodbus.client import ModbusSerialClient
from pymodbus.exceptions import ModbusException

# ============================================================================
# Configuration - adjust these to match your PIC MF40 setup
# ============================================================================
SERIAL_PORT  = '/dev/ttyAMA0'       # RS-485 HAT on Pi 5 (/dev/serial0 -> /dev/ttyAMA0), RP1 chip not CPU(BCM2712)
BAUDRATE     = 57600                # Default baud rate for PIC MF40 : 57600
SLAVE_ID     = 50                   # PIC Modbus slave address (MF40_Userdata.MB_Slave_Adr)
BYTESIZE     = 8
PARITY       = 'N'                  # No parity
STOPBITS     = 1
TIMEOUT      = 3                    # seconds

# ============================================================================
# Register definitions
# ============================================================================

# RW registers (function code 3 CMD sent to PIC32(SLAVE ID 50), starting at address 2)
RW_REGISTERS = {
    2:  "P_SET",
    4:  "TEMP_SET",
    6:  "TIMER_SET",
    8:  "PID_Kp",
    10: "PID_Ki",
    12: "PID_Kd",
    14: "GenOffUse",
    16: "StopAtTemp",
    18: "TimerDelay",
}

# RO registers (function code 3 CMD sent to PIC32(SLAVE ID 50), starting at address 200)
RO_REGISTERS = {
    202: "P",
    204: "TEMP",
    206: "TIMER",
    208: "I",
    210: "FREQ",
    212: "COS_PHI",
    214: "REG%",
}


def registers_to_float(reg_hi, reg_lo):
    """Convert two 16-bit Modbus registers to a 32-bit float (big-endian/word swap).
    
    PIC uses cswap4s with big-endian byte order: high register first, then low register.

    >(Greater than sign) = This tells Python to use BIG_ENDIAN format. This means the most significant bytes come first
    H                    = Unsigned short 16-bits
    HH                   = expects two 16-bits = 32 bits integer
    """
    raw_bytes = struct.pack('>HH', reg_hi, reg_lo)
    return struct.unpack('>f', raw_bytes)[0]


def read_holding_register_float(client, address, slave_id=SLAVE_ID):
    """Read a 32-bit float value from two consecutive holding registers.
    
    Args:
        client: Modbus client
        address: Starting register address 
        slave_id: Modbus slave address
    
    Returns:
        float value or None on error
    """
    try:
        result = client.read_holding_registers(address=address, count=2, device_id=slave_id)
        if result.isError():
            print(f"  Error reading address {address}: {result}")
            return None
        return registers_to_float(result.registers[0], result.registers[1])
    except ModbusException as e:
        print(f"  Modbus exception at address {address}: {e}")
        return None


def read_all_ro_registers(client):
    """Read all Read-Only (measurement) registers from PIC MF40."""
    print("\n--- Read-Only Registers (Measured Values) ---")
    for addr, name in sorted(RO_REGISTERS.items()):
        value = read_holding_register_float(client, addr)
        if value is not None:
            print(f"  [{addr:>3d}] {name:<30s} = {value:.4f}")
        else:
            print(f"  [{addr:>3d}] {name:<30s} = READ ERROR")


def read_all_rw_registers(client):
    """Read all Read-Write (setpoint) registers from PIC MF40."""
    print("\n--- Read-Write Registers (Setpoints) ---")
    for addr, name in sorted(RW_REGISTERS.items()):
        value = read_holding_register_float(client, addr)
        if value is not None:
            print(f"  [{addr:>3d}] {name:<30s} = {value:.4f}")
        else:
            print(f"  [{addr:>3d}] {name:<30s} = READ ERROR")


def read_status_registers(client):
    """Read SCADA status registers."""
    print("\n--- Status Registers ---")
    
    # SCADA_Cmd0 at address 0 (uint16)
    '''try:
        result = client.read_holding_registers(address=0, count=1, device_id=SLAVE_ID)
        if not result.isError():
            print(f"  [  0] SCADA_Cmd0               = {result.registers[0]} (0x{result.registers[0]:04X})")
    except ModbusException as e:
        print(f"  Error reading SCADA_Cmd0: {e}")'''

    # TEMP_SET at address 4 (float32)
    temp_set = read_holding_register_float(client, 4)
    if temp_set is not None:
        print(f"  [  4] TEMP_SET                 = {temp_set:.2f}")

    # P at address 2 (float32)
    p = read_holding_register_float(client, 2)
    if p is not None:
        print(f"  [2] P                      = {p:.2f}")

    # TEMP at address 204 (float32)
    temp = read_holding_register_float(client, 204)
    if temp is not None:
        print(f"  [204] TEMP                     = {temp:.2f}")

    # I at address 208 (float32)
    curr = read_holding_register_float(client, 208)
    if curr is not None:
        print(f"  [208] I                      = {curr:.2f}")

    # TL_Busy at address 1300 (uint16)
    TL_busy = read_holding_register_float(client, 300)
    if TL_busy is not None:
        print(f"  [1300] TL_Busy                  = {TL_busy:.2f}")

    # TL_Ready at address 1301 (uint16)
    TL_ready = read_holding_register_float(client, 301)
    if TL_ready is not None:
        print(f"  [1301] TL_ready                 = {TL_ready:.2f}")
    
    # TL_Busy at address 1300 (uint16)
    try:
        result = client.read_holding_registers(address=300, count=1, device_id=SLAVE_ID)
        if not result.isError():
            print(f"  [300] TL_Busy                  = {result.registers[0]} (0x{result.registers[0]:04X})")
    except ModbusException as e:
        print(f"  Error reading TL_Busy: {e}")

    # TL_ready at address 1301 (uint16)
    try:
        result = client.read_holding_registers(address=301, count=1, device_id=SLAVE_ID)
        if not result.isError():
            print(f"  [301] TL_ready                 = {result.registers[0]} (0x{result.registers[0]:04X})")
    except ModbusException as e:
        print(f"  Error reading TL_ready: {e}")
    
    # SCADA_Status0 at address 200 (uint16)
    try:
        result = client.read_holding_registers(address=200, count=1, device_id=SLAVE_ID)
        if not result.isError():
            print(f"  [200] SCADA_Status0            = {result.registers[0]} (0x{result.registers[0]:04X})")
    except ModbusException as e:
        print(f"  Error reading SCADA_Status0: {e}")


def continuous_monitoring(client, interval=2.0):
    """Continuously read temperature and power values."""
    print(f"\n--- Continuous Monitoring (every {interval}s) --- Press Ctrl+C to stop")
    print(f"  {'Time':<12s} {'TEMP':>10s} {'Power':>10s} {'Current':>10s} {'Freq':>10s} {'REG%':>10s}")
    print("  " + "-" * 64)
    
    while True:
        temp  = read_holding_register_float(client, 204)
        power = read_holding_register_float(client, 202)
        curr  = read_holding_register_float(client, 208)
        freq  = read_holding_register_float(client, 210)
        reg   = read_holding_register_float(client, 214)
        
        timestamp = time.strftime("%H:%M:%S")
        print(f"  {timestamp:<12s}"
              f" {temp if temp is not None else 'ERR':>10}"
              f" {power if power is not None else 'ERR':>10}"
              f" {curr if curr is not None else 'ERR':>10}"
              f" {freq if freq is not None else 'ERR':>10}"
              f" {reg if reg is not None else 'ERR':>10}")
        
        time.sleep(interval)


'''
Main program starts here
'''
def main():

    print("=" * 60)
    print("  PI5 Modbus/RTU Master - PIC MF40 Controller")
    print(f"  Port: {SERIAL_PORT}  Baud: {BAUDRATE}  Slave ID: {SLAVE_ID}")
    print("=" * 60)

    # Create Modbus RTU client (Pi is the MASTER, PIC is the SLAVE)
    client = ModbusSerialClient(
        port=SERIAL_PORT,
        baudrate=BAUDRATE,
        bytesize=BYTESIZE,
        parity=PARITY,
        stopbits=STOPBITS,
        timeout=TIMEOUT,
    )

    try:
        # Connect to serial port
        if not client.connect():
            print(f"Error: Could not connect to {SERIAL_PORT}")
            return
        
        print(f"\nConnected to {SERIAL_PORT} successfully.")

        # Read all registers once
        while(True):
            read_status_registers(client)
            time.sleep(2)
            #read_all_rw_registers(client)
            #read_all_ro_registers(client)

        # Start continuous monitoring
        #continuous_monitoring(client, interval=2.0)
    
    except KeyboardInterrupt:
        print("\n\nProgram stopped by user (Ctrl+C).")

    except serial.SerialException as e:
        print(f"\nSerial port error: {e}")

    except Exception as e:
        print(f"\nUnexpected error: {e}")

    finally:
        client.close()
        print("Modbus connection closed.")


if __name__ == "__main__":
    main()
