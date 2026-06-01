#!/home/alphotronic/Desktop/ALPHO/PI5-Modbus-PIC-2026-05-05/venv/bin/python3
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

import csv
import glob
import os
import serial
import struct
import time
from datetime import datetime

from pymodbus.client import ModbusSerialClient
from pymodbus.exceptions import ModbusException

from openpyxl import Workbook
from openpyxl.styles import Font, Color, Alignment, Border, Side, NamedStyle, colors

# ============================================================================
# Bit definitions for status registers
# ============================================================================
BIT0      = 0
BIT1      = 1

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

def create_record_temperatures_excel_table(script_dir, timestamp):

    global workbook

    sheet                 = workbook.active
    header_data           = ('date/time-stamp', 'Set temperature', 'Actual temperature', 'ERROR')
    cell_range            = sheet['A1':'D1']
    row_a                 = sheet[1]
    #apply filter in active sheet
    sheet.auto_filter.ref = "A1:D1"
    # Creating a few styles for the header.
    header                = NamedStyle(name = "header")
    header.font           = Font(bold=True, size = 15)
    header.border         = Border(bottom=Side(border_style="thick"))
    header.alignment      = Alignment(horizontal="center", vertical="center")

    #run loop for header data
    header_data_counter   = 0

    for dataCell in row_a:
        dataCell.style       = header
        dataCell.value       = header_data[header_data_counter]
        header_data_counter  +=1

    #Save the excel file with same naming nomenclature as CSV (TL_data_{timestamp}.xlsx)
    excel_path = os.path.join(script_dir, f"TL_data_{timestamp}.xlsx")
    workbook.save(filename = excel_path)
    print(f"  [TL] Excel saved → {excel_path}")

    

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
    
    # TL_Busy at address 300 (uint16)
    try:
        TL_Busy = client.read_holding_registers(address=300, count=1, device_id=SLAVE_ID)
        if not TL_Busy.isError():
            print(f"  [300] TL_Busy                  = {TL_Busy.registers[0]} (0x{TL_Busy.registers[0]:04X})")
    except ModbusException as e:
        print(f"  Error reading TL_Busy: {e}")

    # TL_ready at address 301 (uint16)
    try:
        TL_ready = client.read_holding_registers(address=301, count=1, device_id=SLAVE_ID)
        if not TL_ready.isError():
            print(f"  [301] TL_ready                 = {TL_ready.registers[0]} (0x{TL_ready.registers[0]:04X})")
    except ModbusException as e:
        print(f"  Error reading TL_ready: {e}")

    '''
            Reading Temperature List registers
            BIG ENDIAN = High Register First, then Low Register
            10000 to 13599 = 3600 registers
            Read only if TL_ready == 1
    '''
    # Extracting bit from TL_BUSY and TL_READY registers
    val_busy  = TL_Busy.registers[0]  if not TL_Busy.isError()  else 0
    val_ready = TL_ready.registers[0] if not TL_ready.isError() else 0

    val_busy_ready = (val_busy << BIT1) | (val_ready << BIT0)
    print(f"  [V] TL_Busy_Ready            = {val_busy_ready} (0x{val_busy_ready:04X})")

    # Initialize static-like variable for status if it doesn't exist
    if not hasattr(read_status_registers, "val_busy_ready_STATUS"):
        read_status_registers.val_busy_ready_STATUS = 0

    if val_busy_ready == 1:
        read_status_registers.val_busy_ready_STATUS = 1

    if read_status_registers.val_busy_ready_STATUS == 1:
        '''
                Reading Production Serial Number registers
                BIG ENDIAN = High Register First, then Low Register
                308 to 323 = 16 registers
                Read only if TL_ready == 1
        '''
        PSN_START  = 308
        PSN_END    = 323

        TL_START   = 10000
        TL_END     = 13599
        TL_COUNT   = TL_END - TL_START + 1   # 3600 registers
        CHUNK_SIZE = 125                      # pymodbus max per request

        print(f"\n  [TL] TL_Ready detected — reading {TL_COUNT} registers ({TL_START}–{TL_END})...")

        tl_rows = []   # list of (address, value_dec, value_hex)
        read_errors = 0

        #--------------------------------------------------------------------
        # Read all 16 PSN registers (308–323) in one request
        #--------------------------------------------------------------------
        psn_string = ""
        try:
            psn_result = client.read_holding_registers(
                address=PSN_START, count=(PSN_END - PSN_START + 1), device_id=SLAVE_ID
            )
            if not psn_result.isError():
                psn_chars = []
                done = False
                for psn_result_val in psn_result.registers:
                    # Each register holds 2 ASCII bytes: high byte first
                    hi_byte = (psn_result_val >> 8) & 0xFF
                    lo_byte =  psn_result_val       & 0xFF
                    for psn_result_val_byte in (hi_byte, lo_byte):
                        if psn_result_val_byte == 0:               # null terminator → stop
                            done = True
                            break
                        if 0x20 <= psn_result_val_byte <= 0x7E:    # printable ASCII only
                            psn_chars.append(chr(psn_result_val_byte))
                    if done:
                        break
                psn_string = "".join(psn_chars)
                print(f"  [PSN] Production Serial Number = {psn_string!r}")
            else:
                psn_string = "READ_ERROR"
                print(f"  [PSN] Error reading PSN registers: {psn_result}")
        except ModbusException as e:
            psn_string = "MODBUS EXCEPTION OCCURRED"
            print(f"  [PSN] Modbus exception reading PSN: {e}")

        # ----------------------------------------------------------------
        # read all 3600 TL registers (10000 – 13599) and write CSV
        # ----------------------------------------------------------------
        addr = TL_START
        while addr <= TL_END:

            chunk = min(CHUNK_SIZE, TL_END - addr + 1)

            try:
                result = client.read_holding_registers(addr, count=chunk, device_id=SLAVE_ID)
                if not result.isError():
                    for i, reg_val in enumerate(result.registers):
                        tl_rows.append((addr + i, reg_val, f"0x{reg_val:04X}"))
                else:
                    print(f"  [TL] Error reading chunk at {addr}: {result}")
                    for i in range(chunk):
                        tl_rows.append((addr + i, "ERR", "ERR"))
                    read_errors += chunk

            except ModbusException as e:
                print(f"  [TL] Modbus exception at {addr}: {e}")
                for i in range(chunk):
                    tl_rows.append((addr + i, "ERR", "ERR"))
                read_errors += chunk

            addr += chunk

        # Always write to a fresh timestamped CSV; remove the previous one first
        # This is the directory of a script
        script_dir    = os.path.dirname(os.path.abspath(__file__))
        # glob.glob is a file search function. It finds all files matching a wildcard pattern 
        # and returns them as a list of paths. 
        # os.path.join is used to join the paths of the directory and the file.
        # * is a wildcard that matches any sequence of characters.
        # TL_data_*.csv will match all files starting with TL_data_ and ending with .csv
        # The list of existing CSVs is stored in the variable existing_csvs.
        existing_csvs = glob.glob(os.path.join(script_dir, "TL_data_*.csv"))
        for old_csv in existing_csvs:          # remove old file(s) before writing
            os.remove(old_csv)

        # Remove old Excel files before writing a new one (same cleanup as CSV)
        existing_xlsxs = glob.glob(os.path.join(script_dir, "TL_data_*.xlsx"))
        for old_xlsx in existing_xlsxs:
            os.remove(old_xlsx)

        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        csv_path  = os.path.join(script_dir, f"TL_data_{timestamp}.csv")

        try:
            with open(csv_path, "w", newline="") as csv_file:
                writer = csv.writer(csv_file)
                # Write PSN as a metadata row at the top of the CSV
                writer.writerow(["PSN", psn_string])
                writer.writerow(["Address", "Temperature_Value_Dec", "Temperature_Value_Hex"])
                # Write Temperature set and actual values
                writer.writerows(tl_rows)
            print(f"  [TL] CSV saved → {csv_path}  ({len(tl_rows)} rows, {read_errors} errors)")
        except OSError as e:
            print(f"  [TL] Failed to write CSV: {e}")

        # Generate Excel Table and record set/measured temperature values
        create_record_temperatures_excel_table(script_dir, timestamp)

        # Reset status so we don't re-read until next TL_Ready pulse
        read_status_registers.val_busy_ready_STATUS = 0
    
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
            time.sleep(1)
            #read_all_rw_registers(client)
            #read_all_ro_registers(client)
    
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

    # Excel workbook for recording temperature data
    workbook = Workbook()
    main()
