import serial
import time
import random

PZEM_PORT = "/dev/ttyUSB0" 
PZEM_BAUDRATE = 9600

ZIGBEE_PORT = "/dev/ttyUSB1"
ZIGBEE_BAUDRATE = 9600

PZEM_REQUEST = bytearray([0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xCB])

def read_pzem_data(serial_conn):
    """Read data from PZEM sensor via Modbus RTU."""
    try:
        serial_conn.write(PZEM_REQUEST)
        time.sleep(0.1)

        response = serial_conn.read(9)
        if len(response) < 9:
            print("Error: Incomplete response from PZEM")
            return None

        voltage = ((response[3] << 8) | response[4]) / 10.0
        current = ((response[5] << 8) | response[6]) / 100.0
        power = ((response[7] << 8) | response[8]) / 10.0
        energy = ((response[9] << 8) | response[10]) / 1000.0

        return voltage, current, power, energy
    except Exception as e:
        print(f"Error reading PZEM data: {e}")
        return None

def send_zigbee_message(serial_conn, message):
    try:
        serial_conn.write(message.encode() + b"\n")
        print(f"Sent Zigbee message: {message}")
    except Exception as e:
        print(f"Error sending Zigbee message: {e}")

def main():
    try:
        pzem_serial = serial.Serial(PZEM_PORT, PZEM_BAUDRATE, timeout=1)
        print(f"Connected to PZEM on {PZEM_PORT}")
    except Exception as e:
        print(f"Failed to connect to PZEM: {e}")
        return

    try:
        zigbee_serial = serial.Serial(ZIGBEE_PORT, ZIGBEE_BAUDRATE, timeout=1)
        print(f"Connected to Zigbee module on {ZIGBEE_PORT}")
    except Exception as e:
        print(f"Failed to connect to Zigbee module: {e}")
        return

    while True:
        pzem_data = read_pzem_data(pzem_serial)
        if pzem_data:
            voltage, current, power, energy = pzem_data
            print(f"Voltage: {voltage:.2f} V, Current: {current:.2f} A, Power: {power:.2f} W, Energy: {energy:.4f} kWh")

            message = f"Voltage: {voltage:.2f} V, Current: {current:.2f} A, Power: {power:.2f} W, Energy: {energy:.4f} kWh"

            send_zigbee_message(zigbee_serial, message)

        time.sleep(1)

if __name__ == "__main__":
    main()
