import serial
import time

# Replace with your COM port
PORT = 'COM4'  # For Windows (check Device Manager)
# PORT = '/dev/ttyUSB0'  # For Linux/macOS

BAUD_RATE = 9600

ser = serial.Serial(PORT, BAUD_RATE, timeout=1)

time.sleep(2)  # wait for connection to settle

print("Starting to read from HuskyLens...")

try:
    while True:
        if ser.in_waiting:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            print("Received:", line)
except KeyboardInterrupt:
    print("Exiting...")
finally:
    ser.close()
