import serial

#port = serial.Serial("/dev/ttyS0", baudrate=9600, timeout=3.0)  # Use this port only when using Raspberry Pi 4
port = serial.Serial("/dev/tty0", baudrate=9600, timeout=3.0)    # Use this port only when using Raspberry Pi 5

print("Starting test...")

while True:
    port.write("\r\n RS485-Test".encode('utf-8'))
    #rcv = port.read(10)
    #port.write(("\r\nYou sent:" + repr(rcv)).encode('utf-8'))