'''import serial
import threading

# Replace these with your actual COM ports
msp432_port = '/dev/ttyACM0'  # Port for MSP432
esp8266_port = '/dev/ttyUSB0'  # Port for ESP8266
baud_rate = 115200

# Open serial connections
msp432_serial = serial.Serial(msp432_port, baud_rate, timeout=1)
esp8266_serial = serial.Serial(esp8266_port, baud_rate, timeout=1)

# Event to signal threads to stop
stop_event = threading.Event()

def forward_data(source, destination):
    while True:
        if source.in_waiting > 0:
            data = source.read(source.in_waiting)
            destination.write(data)
            print(f"Forwarded {data} from {source.port} to {destination.port}")

# Create threads to handle bidirectional communication
thread1 = threading.Thread(target=forward_data, args=(msp432_serial, esp8266_serial))
thread2 = threading.Thread(target=forward_data, args=(esp8266_serial, msp432_serial))

thread1.start()
thread2.start()

# Keep the main thread alive to allow threads to run
try:
    while True:
        pass
except KeyboardInterrupt:
    print("Stopping serial bridge")
finally:
    # Set stop event to signal threads to stop
    stop_event.set()
    msp432_serial.close()
    esp8266_serial.close()
    thread1.join()
    thread2.join()
'''
import serial
import time
import threading

# Update these port names to match your setup
msp432_port = '/dev/ttyACM0'  # The COM port for your MSP432
esp8266_port = '/dev/ttyUSB0'  # The COM port for your ESP8266

# Open serial ports
msp432_serial = serial.Serial(msp432_port, 115200, timeout=1)
esp8266_serial = serial.Serial(esp8266_port, 115200, timeout=1)

time.sleep(2)  # Wait for the serial connections to initialize

def forward_data(src_serial, dest_serial):
    while True:
        while src_serial.in_waiting > 0:
            try:
                data = src_serial.read(1)  # Read one byte at a time
                if data:
                    print(f"Forwarding data from {src_serial.port}: {data}")
                    dest_serial.write(data)
            except Exception as e:
                print(f"Error reading or forwarding data from {src_serial.port}: {e}")

def read_responses(serial_port):
    while True:
        while serial_port.in_waiting > 0:
            try:
                data = serial_port.read(1)  # Read one byte at a time
                if data:
                    print(f"Received response from {serial_port.port}: {data}")
            except Exception as e:
                print(f"Error reading response from {serial_port.port}: {e}")

# Creating threads to handle communication
threads = [
    threading.Thread(target=forward_data, args=(msp432_serial, esp8266_serial)),
    threading.Thread(target=forward_data, args=(esp8266_serial, msp432_serial)),
    threading.Thread(target=read_responses, args=(msp432_serial,)),
    threading.Thread(target=read_responses, args=(esp8266_serial,))
]

# Start threads
for thread in threads:
    thread.start()

try:
    # Keep the main thread alive to let the other threads run
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    print("Exiting program.")
finally:
    # Close serial ports
    msp432_serial.close()
    esp8266_serial.close()
