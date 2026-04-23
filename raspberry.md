# Raspberry Pi 5 - Debian Setup Guide

This document provides step-by-step instructions for installing Debian-based Raspberry Pi OS on a Raspberry Pi 5 using a 32GB SD card.

## 1. Prerequisites
- **Hardware**: Raspberry Pi 5.
- **Storage**: 32 GB MicroSD Card (Class 10 or UHS-I recommended for performance).
- **Peripheral**: MicroSD card reader for your PC/Laptop.
- **Software**: Raspberry Pi Imager (Downloaded from [raspberrypi.com](https://www.raspberrypi.com/software/)).

---

## 2. Hardware Specifics for Raspberry Pi 5
- **Power Button**: Unlike previous models, the Pi 5 has a dedicated power button on the side. 
  - Single press (when on): Initiates a safe shutdown.
  - Single press (when off): Powers on the Pi.
- **Cooling**: The Pi 5 runs significantly hotter than the Pi 4. It is **strongly recommended** to use the official "Active Cooler" or a case with a fan. Without cooling, the CPU will throttle quickly.
- **Power Supply**: Use the official Raspberry Pi 27W USB-C Power Supply. Using a standard 5V/3A (15W) phone charger may limit USB current to 600mA and might cause stability issues.

## 3. Installation Steps using Raspberry Pi Imager

### Step 1: Install Raspberry Pi Imager
1. Download the version for Windows from the official website.
2. Install and launch the application.

### Step 2: Choose Device
1. Click on **CHOOSE DEVICE**.
2. Select **Raspberry Pi 5** from the list. This ensures the Imager filters for compatible OS versions.

### Step 3: Choose Operating System
1. Click on **CHOOSE OS**.
2. Select **Raspberry Pi OS (64-bit)**. 
   - *Note: This is the Debian-based official OS recommended for Raspberry Pi 5.*
   - If you need a desktop environment, choose the standard version. For a lightweight server-style setup, choose **Raspberry Pi OS Lite (64-bit)** under "Raspberry Pi OS (other)".

### Step 4: Choose Storage
1. Insert your 32GB SD card into your PC.
2. Click on **CHOOSE STORAGE**.
3. Select your SD card from the list. **WARNING: All existing data on this card will be deleted.**

### Step 5: OS Customization (Crucial)
After clicking **NEXT**, a popup will ask if you want to apply OS customization settings. Click **EDIT SETTINGS**.
1. **General Tab**:
   - **Set hostname**: `ALPHOTRONICpi`
   - **Set username and password**: Use `alphotronic` for both as per your design.
   - **Configure wireless LAN**: Enter your SSID (WiFi Name) and Password.
   - **Set locale settings**: Choose your Time zone and Keyboard layout.
2. **Services Tab**:
   - **Enable SSH**: Select "Use password authentication". This allows you to log in remotely without a monitor.
3. Click **SAVE**.

### Step 6: Write to SD Card
1. Click **YES** to apply the customization settings.
2. Click **YES** to confirm that the SD card will be erased.
3. Wait for the process to finish (Writing and Verifying).
4. Once completed, you can safely remove the SD card from your PC.

---

## 3. First Boot Configuration

1. Insert the MicroSD card into the slot on the Raspberry Pi 5.
2. Connect a USB-C power supply (Official 27W supply recommended for Pi 5).
3. The Pi will boot. The first boot takes a bit longer as it expands the file system.
4. **Accessing the Pi**:
    - **Headless (via Network)**: If you enabled SSH and WiFi, wait 2-3 minutes, then open a terminal on your PC and type:
      ```bash
      # Using hostname
      ssh alphotronic@ALPHOTRONICpi.local
      
      # Or using your static IP
      ssh alphotronic@192.168.178.89
      ```
    - **With Monitor**: Connect via Micro-HDMI to see the desktop/console.

### Note on Raspberry Pi Connect
You **do not** need to activate Raspberry Pi Connect now. 
- **What is it?**: A secure way to access your Pi's desktop or terminal via a web browser from anywhere in the world.
- **Can I do it later?**: Yes, you can install and activate it at any time once your Pi is up and running.
- **Recommendation**: For initial development, **SSH** (which we enabled in Step 5) is faster and more direct. Use Raspberry Pi Connect later if you need remote access without setting up a VPN or Port Forwarding.

---

## 4. Post-Installation Commands
Once logged in, run these commands to ensure your system is up to date:
```bash
sudo apt update
sudo apt upgrade -y
```

---

## 5. Installing Additional Software (e.g., Pluma Editor)
If you want to install a lightweight graphical text editor like **Pluma**, run:
```bash
sudo apt update
sudo apt install pluma -y
```
To open it from the terminal: `pluma` or `pluma <filename>`.

---

## 6. Development Environment & Editor Recommendations
As you transition from STM32 development, choosing the right editor is key:

### 1. VS Code (Recommended)
**Best for**: Professional C++, Python, and Project Management.
- **Workflow**: Install VS Code on your Windows PC and use the **Remote - SSH** extension to connect to your Pi.
- **Pros**: Full IntelliSense, debugging tools, git integration, and a massive library of extensions.
- **Cons**: Requires a PC to act as the host (unless running locally on the Pi 5, which is possible but uses more RAM).

### 2. Geany (Best Lightweight IDE)
**Best for**: Fast, local development directly on the Pi.
- **Installation**: `sudo apt install geany -y`
- **Pros**: Extremely fast, supports C/C++ build systems, and is very lightweight.
- **Cons**: Less "modern" UI than VS Code.

### 3. Thonny
**Best for**: Python specifically.
- **Pros**: Pre-installed, excellent for beginners, and has a built-in variable explorer.
- **Cons**: Not suitable for C/C++.

### 4. Vim or Nano
**Best for**: Quick terminal edits.
- **Pros**: No GUI needed; works over a basic SSH terminal.
- **Cons**: Steep learning curve for Vim; Nano is very basic.

---

## 7. Comparison: STM32F446 vs Raspberry Pi 5
| Feature | STM32F446 | Raspberry Pi 5 |
| :--- | :--- | :--- |
| **Type** | Microcontroller (MCU) | Single Board Computer (SBC) |
| **OS** | Bare Metal / RTOS (FreeRTOS) | Linux (Debian) |
| **Clock Speed** | 180 MHz | 2.4 GHz (Quad-core) |
| **RAM** | 128 KB | 4GB / 8GB |
| **Storage** | 512 KB Flash | SD Card / NVMe SSD |
| **Programming** | C/C++ (HAL/LL) | Python, C++, Java, Node.js, etc. |

---

## 8. GPIO Voltage Levels & Hardware Safety

**CRITICAL WARNING**: Unlike many older microcontrollers (like some Arduino models) that use 5V logic, the Raspberry Pi uses **3.3V logic** for its GPIO pins.

- **3.3V Pins**: All GPIO pins (General Purpose Input/Output) operate at **3.3V**.
- **5V Tolerance**: The GPIO pins are **NOT 5V tolerant**. Connecting a 5V signal directly to a GPIO pin will likely destroy the pin and could permanently damage the entire Raspberry Pi.
- **Power Pins**: The 40-pin header *does* include two **5V power pins** (Pins 2 and 4) and two **3.3V power pins** (Pins 1 and 17). 
  - These are for powering external components, but they are separate from the logic/data pins.

| Feature | Voltage Level |
| :--- | :--- |
| **GPIO Logic High** | 3.3V |
| **GPIO Logic Low** | 0V |
| **GPIO Max Current** | ~16mA per pin (Total ~50mA across all pins recommended) |
| **Power Output Pins** | 5V and 3.3V available |

**Safe Interfacing**: If you need to connect a 5V sensor or device to your Pi, you must use a **Logic Level Shifter** or a voltage divider to step the signal down to 3.3V.

---

## 9. Hardware Integration: Joy-IT RB-RS485

The **RB-RS485** is a HAT-style board designed to connect directly to the Raspberry Pi's 40-pin GPIO header.

### 9.1 Physical Connection
1.  **Mounting (26-pin to 40-pin Alignment)**: The RB-RS485 uses a 26-pin header.
    - **Pin 1 to Pin 1**: You must align **Pin 1 of the board** with **Pin 1 of the Pi 5** (the pin with the square pad).
    - Pins 27 to 40 on the Pi 5 will remain **uncovered**.
    - Carefully press down firmly to seat the board.
2.  **Logic Pins (UART)**: The board connects to the Pi's primary UART. Note that the labels on the RB-RS485 board are from the board's perspective (TX/RX crossover):
    - **Physical Pin 8 (GPIO 14)**: Raspberry Pi **TX** -> Connects to RB-RS485 **RX**.
    - **Physical Pin 10 (GPIO 15)**: Raspberry Pi **RX** -> Connects to RB-RS485 **TX**.
3.  **Power**: It draws 5V, 3.3V, and GND from the header.
4.  **Expansion**: The additional pins labeled **P0 to P7** on the board are pass-through headers for other GPIOs (e.g., P0 = GPIO17, P1 = GPIO18).

### 9.2 Software Configuration (Linux)
On the Raspberry Pi 5, the serial port must be enabled in the OS:

1.  **Enable Serial Port**:
    - Run `sudo raspi-config`.
    - Go to **3 Interface Options** -> **I6 Serial Port**.
    - **Login shell over serial?** -> Select **No**.
    - **Serial port hardware enabled?** -> Select **Yes**.
    - **Finish** and **Reboot**.
2.  **Port Name**: After rebooting, the RS485 interface will be accessible at:
    - Path: `/dev/ttyAMA0` (or `/dev/serial0`)
3.  **Verification**:
    ```bash
    ls -l /dev/serial0
    # Should point to ttyAMA0
    ```

### 9.3 Modbus RTU Wiring
- **DB9 Port**: Use Pin 2 (A) and Pin 3 (B).
- **Screw Terminals**: Use the terminals labeled **A** and **B**.
- **Termination**: If this is the end of your Modbus chain, ensure the 120Ω termination is active (often handled via a jumper on the board if present).

### 9.4 Pin Mapping Reference

#### 26-Pin Connection Header (Pi Side)
| Pi Pin | Function | Description |
| :--- | :--- | :--- |
| **Pin 1** | **3.3V** | Power supply for board logic. |
| **Pin 2** | **5V** | Power supply for RS485 transceiver. |
| **Pin 6** | **GND** | Common Ground. |
| **Pin 8** | **UART TX** | Pi TX -> Board RX. |
| **Pin 10** | **UART RX** | Pi RX -> Board TX. |

#### 10-Pin Expansion Header (Board Top)
| Board Label | Pi GPIO | Physical Pi Pin |
| :--- | :--- | :--- |
| **3.3V** | 3.3V Power | Pin 1 |
| **GND** | Ground | Pin 6 / 9 |
| **P0** | **GPIO 17** | Pin 11 |
| **P1** | **GPIO 18** | Pin 12 |
| **P2** | **GPIO 27** | Pin 13 |
| **P3** | **GPIO 22** | Pin 15 |
| **P4** | **GPIO 23** | Pin 16 |
| **P5** | **GPIO 24** | Pin 18 |
| **P6** | **GPIO 25** | Pin 22 |
| **P7** | **GPIO 4** | Pin 7 |

---

## 10. Remote Desktop Access (GUI)

If you need a graphical desktop on Windows without an HDMI monitor:

### 10.1 Installing RDP (Remote Desktop Protocol)
1.  **Install xrdp**: `sudo apt install xrdp -y`
2.  **Permissions**: `sudo adduser alphotronic ssl-cert`
3.  **Troubleshooting (Window closes after login)**:
    - **Log out** of any local sessions on the physical Pi monitor.
    - **Switch to X11**: RDP works better with X11 on Pi 5. Run `sudo raspi-config` -> **Advanced Options** -> **Wayland** -> **X11**.

---

## 11. Development Environments

| Feature | VS 2019 Professional | VS Code (Recommended) |
| :--- | :--- | :--- |
| **Primary Language** | C++ (Excellent) | Python (Excellent) |
| **Remote Workflow** | SSH Build / GDBServer | Remote - SSH Extension |
| **Setup Complexity** | High | Low |
| **Pi 5 Support** | Manual Configuration | Plug-and-Play |

---

## 12. Recommended Workflow: VS Code Remote - SSH

This is the most efficient way to develop Python on the Raspberry Pi 5.

### 12.1 Setup Steps
1.  **Windows**: Install **VS Code** and the **Remote - SSH** extension.
2.  **Connect**: Click the green icon (bottom-left) -> **Connect to Host...** -> `alphotronic@<IP>`.
3.  **Python Extension**: Once connected to the Pi, go to the Extensions tab and install the **Python** extension *on the remote SSH host*.
4.  **Interpreter**: Open your project folder and select the Pi's Python interpreter (bottom-right status bar).

---

**Summary**: You have now replaced the STM32F446 firmware logic with a full Debian Linux environment and configured the hardware for RS485 Modbus RTU communication. Your development environment is set up with VS Code Remote - SSH for Python development.
