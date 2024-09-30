import serial
import threading

import argparse

class SerialTerminal:
    def __init__(self, port='/dev/ttyUSB0', baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.serial = serial.Serial(port, baudrate, timeout=1)
        self.thread = threading.Thread(target=self.read_serial)
        self.thread.daemon = True
        self.thread.start()

    def read_serial(self):
        while True:
            data = self.serial.readline()
            if data:
                print(data.decode('utf-8', errors='ignore'))

    def write_serial(self, data):
        self.serial.write(data.encode('utf-8'))

    def close(self):
        self.serial.close()

def main():

    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", help="Введите номер порта", required=True)
    parser.add_argument("-b", "--baud", help="Введите скорость передачи", type=int, default=115200)

    args = parser.parse_args()

    port = args.port
    baudrate = args.baud

    terminal = SerialTerminal(port, baudrate)

    while True:
        command = input()
        if command.lower() == 'exit' or command.lower() == 'q':
            terminal.close()
            break
        elif command:
            command = f'{command}\r\n'
            terminal.write_serial(command)

if __name__ == '__main__':
    main()