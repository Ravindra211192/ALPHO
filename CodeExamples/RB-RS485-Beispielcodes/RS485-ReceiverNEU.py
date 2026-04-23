import serial

#port = serial.Serial("/dev/ttyS0", baudrate=9600, timeout=3.0)  # Use this port only when using Raspberry Pi 4
port = serial.Serial("/dev/tty0", baudrate=9600, timeout=3.0)    # Use this port only when using Raspberry Pi 5

while True:
    rcv = port.readline()
    print(rcv)