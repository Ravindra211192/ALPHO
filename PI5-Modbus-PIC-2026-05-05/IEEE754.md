# IEEE 754 Single-Precision (32-bit) Floating-Point Format

The **IEEE 754** standard is a universal method used by almost all modern computers and microcontrollers (like PIC and Raspberry Pi) to represent floating-point numbers (numbers with fractions or decimals) in binary. 

In Modbus communications, 32-bit floating-point numbers span across two 16-bit holding registers. 

## The Anatomy of a 32-bit Float
A 32-bit float breaks a number down into three distinct parts, much like scientific notation (e.g., $+1.5625 \times 10^4$), but in base-2 (binary).

The 32 bits are divided as follows:
1. **Sign bit (1 bit):** Determines if the number is positive or negative.
2. **Exponent (8 bits):** Determines the multiplier (the power of 2) moving the decimal point left or right.
3. **Mantissa / Fraction (23 bits):** Represents the core digits of the number.

---

## Step-by-Step Example

Let's look at a simplified version of a temperature reading: **`25.0`**. 
In Modbus logs, `25.0` might be represented across two 16-bit registers as `0x41C8` (High Word) and `0x0000` (Low Word).
Combined, the 32-bit hexadecimal value is **`0x41C80000`**. 

Let's convert that Hexadecimal value into binary (32 bits):
`0x41C80000` = **`0100 0001 1100 1000 0000 0000 0000 0000`**

Now, let's chop those 32 bits into the 3 IEEE 754 components:

### 1. The Sign Bit (Bit 31)
* **Binary:** `0`
* **Meaning:** `0` means the number is **positive (+)**. If it were `1`, the number would be negative.

### 2. The Exponent (Bits 30 to 23)
* **Binary:** `10000011`
* **Decimal Value:** In decimal, `10000011` is **131** (128 + 2 + 1).
* **The "Bias":** To allow for both negative and positive exponents (to represent huge numbers and tiny fractions), IEEE 754 subtracts a standard "bias" of **127** from this value.
* **True Exponent:** 131 - 127 = **4**
* **Meaning:** We will multiply our base number by 2^4 (which is 16).

### 3. The Mantissa (Bits 22 to 0)
* **Binary:** `10010000000000000000000`
* **The "Hidden 1":** In binary scientific notation, a valid number always starts with a `1` (e.g., `1.something`). To save space, IEEE 754 *assumes* there is a `1.` at the front, so it isn't stored. We have to add it back!
* **True Mantissa:** **`1.`**`1001000000...`
* **Converting to Decimal:** Just like decimals after a dot represent fractions of 10 (1/10, 1/100), binary digits after a dot represent fractions of 2 (1/2, 1/4, 1/8, 1/16...)
   * $1$ (the hidden 1)
   * $+$ $1 \times (1/2)$ = 0.5
   * $+$ $0 \times (1/4)$ = 0
   * $+$ $0 \times (1/8)$ = 0
   * $+$ $1 \times (1/16)$ = 0.0625
* **Sum:** $1 + 0.5 + 0.0625$ = **`1.5625`**

### Final Calculation
Now we put it all together using the formula: 
`Sign * Mantissa * (2 ^ Exponent)`

$$ +1 \times 1.5625 \times 2^4 $$
$$ 1.5625 \times 16 = \mathbf{25.0} $$

---

## Modbus Context
Modbus was designed around 16-bit chunks of memory (Holding Registers). Because an IEEE 754 float requires 32 bits, the Modbus Master must read **two consecutive 16-bit registers**, combine them into a single 32-bit sequence, and then perform the math shown above to decode the actual value.
