[11.05.2026, 12:50]


I tested the script "uart_loopback_test.py" on PI5 without HAT RB-RS485. I got FAILURE message, stating Sending: HEARTBEAT_TEST_12345
FAILURE: No data received. Check your jumper wire connection.

I changed Port to /dev/ttyAMA10 but I got same result.

I updated config.txt(devicetree overlay) : dtoverlay=miniuart-bt  but still not working.

PORT = '/dev/ttyAMA10' // Please don't use /dev/ttyAMA0 port.
BAUDRATE = 9600

ls -l '/dev/serial0' : lrwxrwxrwx 1 root root 8 May 11 14:40 /dev/serial0 -> ttyAMA10
This means /dev/serial0 points to /dev/ttyAMA10. But we use /dev/ttyAMA0 in the code.

============================================================
  PI5 Modbus/RTU Master - PIC MF40 Controller
  Port: /dev/ttyAMA0  Baud: 57600  Slave ID: 50
============================================================

Connected to /dev/ttyAMA0 successfully.

--- Status Registers ---
  [  0] SCADA_Cmd0               = 0x0000
  [4] TEMP_SET            = 0x42CE
  [4] TEMP_SET            = 0xF227
  [204] TEMP               = 0x41C8
  [204] TEMP               = 0x0061
  [200] SCADA_Status0             = 0x1010
Modbus connection closed.