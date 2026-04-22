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

**Summary**: You have now replaced the STM32F446 firmware logic with a full Debian Linux environment. Your next steps will involve porting your peripheral logic (UART, SPI, I2C) to the Linux-equivalent drivers (e.g., `/dev/ttyAMA0`, `/dev/spidev0.0`).
