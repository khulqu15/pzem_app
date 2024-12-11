import time
import serial
import random

SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 115200

ZIGBEE_DEVICE_ADDRESS = "0x00124B0000000001"

def send_zigbee_data(ser, message):
    try:
        command = f"AT+SEND={ZIGBEE_DEVICE_ADDRESS},{message}\r\n"
        ser.write(command.encode())
        response = ser.readline().decode().strip()
        print(f"Zigbee response: {response}")
    except Exception as e:
        print(f"Error sending Zigbee data: {e}")

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} bps.")

        while True:
            voltage = random.uniform(220.0, 221.0)
            current = random.uniform(14.0, 15.0)
            power = voltage * current
            energy = power / 3600 / 1000

            message = f"Voltage: {voltage:.2f} V, Current: {current:.2f} A, Power: {power:.2f} W, Energy: {energy:.4f} kWh"
            print(f"Sending message: {message}")

            send_zigbee_data(ser, message)

            time.sleep(1)

    except serial.SerialException as e:
        print(f"Serial connection error: {e}")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serial connection closed.")

if __name__ == "__main__":
    main()
