# ALPHO Project: High-Throughput RS485 to USB Bridge (Pi 5 Version)

This project implements a high-performance bridge between RS485 (Modbus RTU) and USB Mass Storage, powered by the **Raspberry Pi 5**. It is designed for large-scale data logging applications where data from external MCUs needs to be buffered and stored efficiently on a USB stick in CSV format.

## System Architecture

```mermaid
graph LR
    A[Microchip Source MCU] -- RS485 / Modbus RTU --> B[Raspberry Pi 5 Bridge]
    B -- USB 3.0 Host --> C[USB Stick / SSD\nCSV Files]
```

## Technical Hardware Design

### 1. RS485 Interface
- **UART Interface**: Primary UART on GPIO 14 (TX) and GPIO 15 (RX).
- **Transceiver**: SP3485 or similar 3.3V RS485 transceiver.
- **Direction Control**: Handled via a dedicated GPIO (e.g., GPIO 18) for Half-Duplex DE/RE switching.
- **Isolation**: Recommended for industrial environments to protect the Pi 5 from noise and ground loops.

### 2. USB Interface
- **Storage**: Pi 5 utilizes high-speed USB 3.0 ports, allowing for significantly higher write speeds compared to traditional MCUs.
- **Power**: The Pi 5 provides up to 1.6A of current to USB devices (with the official 27W supply), making it capable of powering high-speed USB sticks or NVMe SSDs via USB adapters.

## Software Architecture

### Core Stack
- **OS**: Raspberry Pi OS (Debian 64-bit).
- **Languages**: Python 3.13+ or C++ (using `libgpiod` and `serial` libraries).
- **File System**: Standard Linux POSIX file handling (ext4 or FAT32/exFAT for the USB stick).

### Optimization Strategies
- **Linux Page Cache**: The OS automatically handles multi-buffering and write optimization to minimize disk latency.
- **High-Performance CPU**: The 2.4 GHz Quad-core processor easily handles the conversion of 1,800+ floats to CSV strings with negligible CPU load.
- **Modbus Protocol**: Implements Modbus RTU Master logic using industrial-grade libraries (e.g., `pymodbus` or `libmodbus`).

## Feasibility & Scalability Analysis (1,800 Data Points)

The system is designed to handle up to **1,800 temperature data points** (and beyond) with extreme reliability.

### 1. Memory (RAM) Consumption
*   **Data Footprint**: 1,800 32-bit floats require only **7.2 KB**.
*   **Pi 5 Capacity**: With **4GB or 8GB RAM**, this uses less than **0.001%** of total memory.
*   **Scalability**: The system can buffer millions of data points in RAM if the USB storage is temporarily unavailable.

### 2. Communication Throughput (Modbus RTU)
*   **Protocol Limit**: Since Modbus RTU is limited to 125 registers per query, the Master (Pi 5) performs **15 sequential requests** for 1,800 points.
*   **Refresh Rate**: At **115,200 bps**, a full system sync takes ~400ms. The Pi 5's multi-core architecture allows acquisition and file writing to happen in parallel without performance loss.

### 3. Storage Performance
*   **USB Write Latency**: Non-existent. The Pi 5's USB 3.0 bus (5 Gbps) handles 7.2 KB transfers in microseconds.
*   **Conclusion**: The design is massive overkill for this scale, meaning it can easily scale to **100,000+ data points** or higher polling frequencies.

## Development Process

### Agile & Scrum Methodology
We follow Agile principles to ensure iterative development and high quality:
- **Sprints**: Work is divided into focused cycles (e.g., 2 weeks).
- **Standups**: Daily syncs to track progress.
- **Review/Retrospective**: Analyzing work to improve the next cycle.

### CI/CD & Remote Workflow
- **Remote Development**: VS Code with Remote-SSH or Geany directly on the Pi.
- **Version Control**: GitHub used for code management and deployment.
- **Deployment**: `scp` or `rsync` used to push updates to the Pi 5 bridge.

## Getting Started
1. Clone the repository to the Raspberry Pi 5.
2. Ensure the user is in the `dialout` and `gpio` groups.
3. Install dependencies: `pip install -r requirements.txt`.
4. Run the main bridge script: `python main.py`.
