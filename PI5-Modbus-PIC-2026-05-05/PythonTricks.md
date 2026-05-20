# Python Tricks and Techniques

This document contains useful Python tricks, patterns, and techniques used in the Modbus Master project.

## 1. Bitwise Operations (Packing Bits)
In Python, bitwise operations function identically to C/C++. You can define bit constants and use the left shift (`<<`) and bitwise OR (`|`) operators to pack multiple flag values into a single state variable.

```python
# Define bit positions
BIT0 = 0
BIT1 = 1

# Example values (e.g., read from Modbus, where 0 is off and 1 is on)
val_busy = 1
val_ready = 1

# Pack bits into a single integer
# val_busy is shifted left by 1 (position 1)
# val_ready is shifted left by 0 (position 0)
val_busy_ready = (val_busy << BIT1) | (val_ready << BIT0)

print(f"Packed State = {val_busy_ready} (0x{val_busy_ready:04X})") 
# If both are 1, val_busy_ready = 3 (binary 11)
```

## 2. Static-Like Variables inside Functions
Python doesn't have a `static` keyword for local variables inside functions like C or C++ does. If you want a variable to persist its state across multiple function calls without making it a global variable, you can attach it as an attribute to the function object itself.

```python
def read_status_registers():
    # Initialize static-like variable if it doesn't exist
    if not hasattr(read_status_registers, "TL_Address"):
        read_status_registers.TL_Address = 10000
        
    print(f"Current Address: {read_status_registers.TL_Address}")
    
    # Update for the next time the function is called
    read_status_registers.TL_Address += 1
```
*Note: This is possible because functions in Python are first-class objects.*

## 3. Safe Extraction from Modbus Responses (Ternary Operator)
When reading from a Modbus device, the response can either be successful or return an error object. Python's conditional expression (ternary operator) is a clean way to handle this on a single line.

```python
# If the read was successful, extract the value. Otherwise, default to 0.
val_busy = TL_Busy.registers[0] if not TL_Busy.isError() else 0
```
This is equivalent to the C ternary operator: `val_busy = !TL_Busy.isError() ? TL_Busy.registers[0] : 0;`

## 4. Packing and Unpacking bytes (Struct module)
When converting a 32-bit float from two 16-bit Modbus registers, you can use the `struct` module to manipulate the raw bytes.

```python
import struct

def registers_to_float(reg_hi, reg_lo):
    # '>HH' = Big-endian (>), two unsigned shorts (H)
    raw_bytes = struct.pack('>HH', reg_hi, reg_lo)
    
    # '>f' = Big-endian (>), float (f)
    return struct.unpack('>f', raw_bytes)[0]
```
