# Autostart - PI_modbus_master.py

## Overview

`PI_modbus_master.py` is configured to **automatically start** when the Raspberry Pi 5 boots up,
using a **systemd service**. systemd is the Linux init system that manages all services and processes
during startup and runtime.

---

## Service File

**File:** `pi_modbus_master.service`

- **Local copy:** `/home/alphotronic/Desktop/ALPHO/PI5-Modbus-PIC-2026-05-05/pi_modbus_master.service`
- **Installed copy:** `/etc/systemd/system/pi_modbus_master.service`

> **Note:** The installed copy in `/etc/systemd/system/` is the one that systemd actually uses.
> If you edit the local copy, you must re-copy it and reload (see [Updating the Service](#updating-the-service) below).

---

## Service File Explained

```ini
[Unit]
Description=PI5 Modbus/RTU Master - PIC MF40 Controller
# Wait until the system is fully booted(multi-user.target) and serial port(dev-ttyAMA0.device) is available
After=multi-user.target dev-ttyAMA0.device
Wants=dev-ttyAMA0.device
```

### `[Unit]` — What this service is and when it should start

| Line                  | Meaning                                                                                                           |
| --------------------- | ----------------------------------------------------------------------------------------------------------------- |
| `Description=...`     | Human-readable name shown in `systemctl status` output                                                            |
| `After=multi-user.target dev-ttyAMA0.device` | **Don't start until** the system is fully booted (`multi-user.target`) **and** the serial port `/dev/ttyAMA0` is available. This prevents the script from crashing because the serial port isn't ready yet |
| `Wants=dev-ttyAMA0.device` | Tells systemd "I'd like this device to be present, but don't fail if it isn't" (soft dependency)             |

---

```ini
[Service]
Type=simple
User=alphotronic
Group=alphotronic
WorkingDirectory=/home/alphotronic/Desktop/ALPHO/PI5-Modbus-PIC-2026-05-05
ExecStart=/home/alphotronic/Desktop/ALPHO/PI5-Modbus-PIC-2026-05-05/venv/bin/python3 /home/alphotronic/Desktop/ALPHO/PI5-Modbus-PIC-2026-05-05/PI_modbus_master.py
Restart=on-failure
RestartSec=5
ExecStartPre=/bin/sleep 5
Environment=USER=alphotronic
```

### `[Service]` — How to run the script

| Line                        | Meaning                                                                                                           |
| --------------------------- | ----------------------------------------------------------------------------------------------------------------- |
| `Type=simple`               | The process started by `ExecStart` **is** the service (it stays running in the foreground — matches the `while(True)` loop in `main()`) |
| `User=alphotronic`          | Run as your user, **not as root**. Important because the script accesses USB drives at `/media/alphotronic/`      |
| `Group=alphotronic`         | Same idea for the group permission                                                                                |
| `WorkingDirectory=...`      | Sets the current directory so file paths in the script resolve correctly                                          |
| `ExecStart=.../venv/bin/python3 .../PI_modbus_master.py` | The actual command to run — uses the **virtual environment's Python** so all pip packages (`pymodbus`, `openpyxl`, etc.) are available |
| `Restart=on-failure`        | If the script crashes (non-zero exit), systemd **automatically restarts** it                                      |
| `RestartSec=5`              | Wait **5 seconds** before restarting after a crash                                                                |
| `ExecStartPre=/bin/sleep 5` | Wait **5 seconds before** starting the script — gives the serial port hardware extra time to initialize after boot |
| `Environment=USER=alphotronic` | Sets the `USER` environment variable — the script uses `os.getenv('USER')` in `get_usb_drives()` to find mounted USB drives |

---

```ini
[Install]
WantedBy=multi-user.target
```

### `[Install]` — When to auto-start

| Line                          | Meaning                                                                                     |
| ----------------------------- | ------------------------------------------------------------------------------------------- |
| `WantedBy=multi-user.target`  | Enable this service in the normal boot sequence (equivalent to "run at startup"). This is what `systemctl enable` hooks into |

---

## Boot Sequence

```
Pi 5 Power On
    │
    ▼
Hardware Init (kernel, drivers)
    │
    ▼
multi-user.target reached (system fully booted)
    │
    ▼
/dev/ttyAMA0 device available (serial port ready)
    │
    ▼
ExecStartPre: sleep 5 seconds (extra safety margin)
    │
    ▼
ExecStart: venv/bin/python3 PI_modbus_master.py  ← Script starts here
    │
    ▼
Script runs continuously (while True loop, polling every 1 second)
    │
    ▼
If script crashes → wait 5 seconds → auto-restart
```

---

## Installation Commands

These commands were used to install and enable the service:

```bash
# 1. Copy the service file to systemd's directory
sudo cp /home/alphotronic/Desktop/ALPHO/PI5-Modbus-PIC-2026-05-05/pi_modbus_master.service /etc/systemd/system/

# 2. Tell systemd to re-scan and pick up the new service file
sudo systemctl daemon-reload

# 3. Enable auto-start on every boot (creates a symlink)
sudo systemctl enable pi_modbus_master.service
```

### What each command does

| Command                           | What it does                                                                                       |
| --------------------------------- | -------------------------------------------------------------------------------------------------- |
| `sudo cp ... /etc/systemd/system/` | Copies the service file into the system directory where Linux looks for service definitions. Like "registering" the script as a system service |
| `sudo systemctl daemon-reload`    | Tells systemd to re-scan its service files and pick up the new/modified `pi_modbus_master.service` |
| `sudo systemctl enable ...`       | Creates a symlink so the service **automatically starts on every boot**                            |

---

## Managing the Service

### Common Commands

| Command                                        | What it does                                       |
| ---------------------------------------------- | -------------------------------------------------- |
| `sudo systemctl start pi_modbus_master`        | Start the service right now (without rebooting)    |
| `sudo systemctl stop pi_modbus_master`         | Stop the running service                           |
| `sudo systemctl restart pi_modbus_master`      | Stop and start again                               |
| `sudo systemctl status pi_modbus_master`       | Check if it's running + see recent logs            |
| `sudo systemctl enable pi_modbus_master`       | Enable auto-start on boot                          |
| `sudo systemctl disable pi_modbus_master`      | Disable auto-start (won't run on boot anymore)     |

### Viewing Logs

| Command                                        | What it does                                       |
| ---------------------------------------------- | -------------------------------------------------- |
| `journalctl -u pi_modbus_master`               | View all logs from the service                     |
| `journalctl -u pi_modbus_master -f`            | Follow live logs (like `tail -f`)                  |
| `journalctl -u pi_modbus_master --since today` | View only today's logs                             |
| `journalctl -u pi_modbus_master -n 50`         | View the last 50 log lines                         |

> **Note:** The script also writes error logs to `PI_modbus_master_error.log` in the script directory.

---

## Updating the Service

If you edit the local `pi_modbus_master.service` file, you must re-copy and reload:

```bash
# 1. Copy updated file
sudo cp /home/alphotronic/Desktop/ALPHO/PI5-Modbus-PIC-2026-05-05/pi_modbus_master.service /etc/systemd/system/

# 2. Reload systemd
sudo systemctl daemon-reload

# 3. Restart the service to apply changes
sudo systemctl restart pi_modbus_master
```

---

## Removing the Service

To completely remove autostart:

```bash
# 1. Stop the service
sudo systemctl stop pi_modbus_master

# 2. Disable auto-start
sudo systemctl disable pi_modbus_master

# 3. Remove the service file
sudo rm /etc/systemd/system/pi_modbus_master.service

# 4. Reload systemd
sudo systemctl daemon-reload
```

---

## Verifying After Reboot

After rebooting the Pi (`sudo reboot`), verify the service is running:

```bash
sudo systemctl status pi_modbus_master
```

Expected output should show:
```
● pi_modbus_master.service - PI5 Modbus/RTU Master - PIC MF40 Controller
     Loaded: loaded (/etc/systemd/system/pi_modbus_master.service; enabled; ...)
     Active: active (running) since ...
```

Key indicators:
- **`enabled`** = will auto-start on boot
- **`active (running)`** = currently running
