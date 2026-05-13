# MODBUS/RTU Communication — Raspberry Pi 5 ↔ PIC MF40

## Overview

This project implements **Modbus/RTU master** communication on a **Raspberry Pi 5**, reading data registers from a **PIC MF40 microcontroller** (slave) via an **RB-RS485 HAT** interface.

```
┌──────────────┐     RS-485 (Half-Duplex)     ┌──────────────┐
│  Raspberry   │◄────────────────────────────►│   PIC MF40   │
│  Pi 5        │     RB-RS485 HAT             │  Controller  │
│  (Master)    │     /dev/serial0             │  (Slave)     │
└──────────────┘                               └──────────────┘
```

---

## Core Modbus Concepts

To understand this project, it is helpful to know a few key Modbus concepts:

- **Modbus Protocol vs. Physical Connection**: Modbus is a software communication protocol. Physically, the network is connected via an **RS-485 serial cable**. The PIC MF40 acts as the **Slave** (answering requests) and the Raspberry Pi acts as the **Master** (initiating requests).
- **RTU (Remote Terminal Unit)**: This refers to the highly efficient **binary data format** used to transmit Modbus messages over the serial cable. It relies on strict timing (silent intervals) to separate messages and uses a 16-bit CRC (Cyclic Redundancy Check) to ensure data integrity over the noisy RS-485 lines.
- **Hardware UART vs. Software RTU Timing**: There is an important distinction between how hardware and software read data. At the physical hardware level, the Pi's UART *does* use a Start Bit and Stop Bit (voltage changes) to frame each individual 8-bit byte. However, at the software protocol level, Modbus RTU does *not* use start or stop characters to frame a complete multi-byte message. Instead, the Modbus software relies on timing: if the serial line goes completely silent for 3.5 character times (~3 milliseconds at 57600 baud), the software knows the current message has ended.
- **Holding Register**: A 16-bit memory slot on the Modbus slave used to store data, configuration values, or setpoints. It is **Read/Write** accessible. In this project, 32-bit float values are created by combining two 16-bit holding registers.
- **Function Code 3 (Read Holding Registers)**: This is the specific command the master sends to ask the slave for the current values stored in a specific block of its holding registers.

---

## Hardware Setup

| Component         | Description                                           |
|-------------------|-------------------------------------------------------|
| **Pi 5 UART**     | `/dev/serial0` → `/dev/ttyAMA10` (GPIO 14/15 pins)   |
| **RS-485 HAT**    | RB-RS485 — plugs onto Pi GPIO header                  |
| **PIC Controller**| PIC MF40 running Modbus/RTU slave firmware (V2.04.02) |
| **Cable**         | RS-485 twisted pair (A/B lines) between HAT and PIC   |

> **⚠ Important — Pi 5 UART Mapping:**  
> On Raspberry Pi 5, the GPIO UART is `/dev/ttyAMA10`, **not** `/dev/ttyAMA0` (which is the debug UART).  
> Always use `/dev/serial0` — it's a symlink that points to the correct device automatically.

---

## Serial Port Configuration

| Parameter   | Value              |
|-------------|-------------------|
| Port        | `/dev/serial0`    |
| Baud Rate   | 57600             |
| Data Bits   | 8                 |
| Parity      | None              |
| Stop Bits   | 1                 |
| Timeout     | 3 seconds         |

---

## Modbus Slave Address

The PIC slave address is configurable via `MF40_Userdata.MB_Slave_Adr` (range: 1–250).  
Default in the Pi script: **Slave ID = 1** — update `SLAVE_ID` in `PI_modbus_master.py` to match your PIC setting.

---

## Register Map (Holding Registers — Function Code 3)

All float values span **2 consecutive registers** (4 bytes, 32-bit IEEE 754 float, big-endian).

### Read-Write Registers (Setpoints) — Address 0–23

| Address | Registers | Name            | Type    | Range            | Description               |
|---------|-----------|-----------------|---------|------------------|---------------------------|
| 0       | 1         | SCADA_Cmd0      | uint16  | —                | Command register          |
| 1       | 1         | SCADA_Cmd1      | uint16  | —                | Command register 2        |
| 2–3     | 2         | P_SET           | float32 | 0 – PSETmax      | Power Setpoint            |
| 4–5     | 2         | TEMP_SET        | float32 | 0 – TEMP_max_SET | Temperature Setpoint      |
| 6–7     | 2         | TIMER_SET       | float32 | 0 – 64800 (18h)  | Timer Setpoint (seconds)  |
| 8–9     | 2         | PID_Kp          | float32 | 0 – 500          | PID Proportional Gain     |
| 10–11   | 2         | PID_Ki          | float32 | 0 – 500          | PID Integral Gain         |
| 12–13   | 2         | PID_Kd          | float32 | 0 – 500          | PID Derivative Gain       |
| 14–15   | 2         | GenOffUse       | uint16* | 0 – 1            | Generator Off Usage       |
| 16–17   | 2         | StopAtTemp      | uint16* | 0 – TEMP_max_SET | Stop at Temperature       |
| 18–19   | 2         | TimerDelay      | uint16* | 0 – 1            | Timer Delay Enable        |
| 20–21   | 2         | Spare           | float32 | -10000 – 10000   | Reserved                  |
| 22–23   | 2         | Spare           | float32 | -10000 – 10000   | Reserved                  |

> *uint16 values are converted to float for Modbus transfer on the PIC side.

### Read-Only Registers (Measurements) — Address 200–215

| Address  | Registers | Name            | Type    | Description                |
|----------|-----------|-----------------|---------|----------------------------|
| 200      | 1         | SCADA_Status0   | uint16  | Status flags               |
| 201      | 1         | SCADA_Status1   | uint16  | LED status                 |
| 202–203  | 2         | P               | float32 | Actual Power               |
| 204–205  | 2         | TEMP            | float32 | Actual Temperature         |
| 206–207  | 2         | TIMER           | float32 | Actual Timer               |
| 208–209  | 2         | I               | float32 | Actual Current             |
| 210–211  | 2         | F               | float32 | Actual Frequency           |
| 212–213  | 2         | COS_PHI         | float32 | Actual Cos-Phi             |
| 214–215  | 2         | REG%            | float32 | Regulator Output (%)       |

### Coil Registers (Function Codes 1, 2, 5, 15)

| Address  | Bits | Name    | Access | Description              |
|----------|------|---------|--------|--------------------------|
| 0–31     | 32   | coil32  | R/W    | Coil bits (code 1 / 5)   |

- **Code 1**: Read Coil Status (bits 0–31)
- **Code 2**: Read Discrete Input Status (bits 16–31 of coil32)
- **Code 5**: Write Single Coil
- **Code 15**: Write Multiple Coils

### Byte Order

- **Default**: Big-endian (network byte order)
- **Little-endian mode**: Add `1000` to the register address (e.g., address `1202` for TEMP in little-endian)
- **Protected write mode**: Use addresses `100–199` (maps to `0–99` with protection bit checking via `SCADA_Cmd1`)

---

## Supported Modbus Function Codes

| Code | Function                    | Supported |
|------|-----------------------------|-----------|
| 1    | Read Coil Status            | ✅        |
| 2    | Read Discrete Input Status  | ✅        |
| 3    | Read Holding Registers      | ✅        |
| 5    | Write Single Coil           | ✅        |
| 6    | Write Single Register       | ✅        |
| 15   | Write Multiple Coils        | ✅        |
| 16   | Write Multiple Registers    | ✅        |

---

## Software Setup on Raspberry Pi 5

### Prerequisites

```bash
# Verify serial port exists
ls -l /dev/serial0 /dev/ttyAMA10

# Verify user is in dialout group (for serial port access)
groups
# Should include: dialout

# If not in dialout group:
sudo usermod -a -G dialout $USER
# Then log out and back in
```

### Python Virtual Environment

The Pi uses an externally-managed Python environment, so a **virtual environment is required**:

```bash
cd ~/Desktop/ALPHO/PI5-Modbus-PIC-2026-05-05

# Create virtual environment (one-time setup)
python3 -m venv venv

# Install dependencies
venv/bin/pip install pymodbus pyserial
```

### Running the Scripts

```bash
# Always use the venv Python to run scripts:
venv/bin/python PI_modbus_master.py

# Or activate the venv first:
source venv/bin/activate
python PI_modbus_master.py

# Simple serial port test (no pymodbus needed):
python3 PI_serial.py
```

> **⚠ Note:** Running `./PI_modbus_master.py` directly will fail with  
> `ModuleNotFoundError: No module named 'pymodbus'`  
> because the shebang (`#!/usr/bin/env python3`) uses the system Python, not the venv.

---

## Project Files

| File                  | Description                                              |
|-----------------------|----------------------------------------------------------|
| `PI_serial.py`        | Basic serial port test — raw read from `/dev/serial0`    |
| `PI_modbus_master.py` | Full Modbus RTU master — reads PIC MF40 registers        |
| `MODBUS.md`           | This documentation file                                  |
| `venv/`               | Python virtual environment (pymodbus + pyserial)         |

---

## Modbus RTU Frame Format

```
┌──────┬──────────┬──────────────────┬────────┐
│ Addr │ Function │ Data             │ CRC-16 │
│ 1B   │ 1B       │ N bytes          │ 2B     │
└──────┴──────────┴──────────────────┴────────┘
```

- **Addr**: Slave address (1 byte)
- **Function**: Function code (1 byte)
- **Data**: Variable length depending on function
- **CRC-16**: 16-bit CRC (low byte first, then high byte)
- **Message timeout**: 3.5 character times (~0.6ms at 57600 baud) — PIC uses 3ms

---

## Troubleshooting

| Problem | Cause | Solution |
|---------|-------|----------|
| `No module named 'pymodbus'` | Using system Python instead of venv | Run with `venv/bin/python` |
| `No response received` | PIC not connected / wrong slave ID / wiring | Check RS-485 A/B lines, verify slave address |
| `Could not open serial port` | Port busy or permissions | Check `ls -l /dev/serial0`, ensure `dialout` group |
| `no display name` (gitk) | No X11 display available | Use `ssh -X` or `git log --oneline --graph --all` |
| Garbled data / wrong values | Byte order mismatch | Try adding 1000 to register address for little-endian |
| `Permission denied` on script | File not executable | Run `chmod +x <script>.py` |

---

## PIC Firmware Reference

- **Source**: `PIC-MF40 - 2022-04-25 V2.04.02/firmware/src/PIC_MODBUS.c`
- **Author**: Röffler Computer Hard+Soft (Hans Röffler)
- **CRC**: CRC-16 via `CRC16str()` function
- **Message Timeout**: 3ms (`MilliSekunden32` based)
