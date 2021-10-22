import threading
import time
import sys
import serial
from maze.main import findPath
from maze.maze_withBLT import Node
from blt.score import Scoreboard
class bluetooth:
    def __init__(self, port: str, baudrate: int=9600):
        """ Initialize an BT object, and auto-connect it. """
        # The port name is the name shown in control panel
        # And the baudrate is the communication setting, default value of HC-05 is 9600.
        self.ser = serial.Serial(port, baudrate=baudrate)
        self.port = port
        
    def is_open(self) -> bool:
        return self.ser.is_open

    def waiting(self) -> bool:
        return self.ser.in_waiting

    def do_connect(self, port: str, baudrate: int=9600) -> bool:
        """ Connect to the specify port with particular baudrate """
        # Connection function. Disconnect the previous communication, specify a new one.
        self.disconnect()

        try:
            self.ser = serial.Serial(port, baudrate=baudrate)
            return True
        except:
            return False

    def disconnect(self):
        """ Close the connection. """
        self.ser.close()

    def write(self, output: str):
        # Write the byte to the output buffer, encoded by utf-8.
        send = output.encode("utf-8")
        self.ser.write(send)

    def readString(self) -> str:
        # Scan the input buffer until meet a '\n'. return none if doesn't exist.
        if(self.waiting()):
            receiveMsg = self.ser.readline().decode("utf-8")[:-1]

        return receiveMsg

def read():
    # scoreboard = Scoreboard('filepath','team',1)
    id = ''
    while True:
        time.sleep(0.01)
        if bt.waiting():
            temp = bt.readString()
            print('temp',temp)
            id += temp if len(temp) == 2 else ('0' + temp)
            if len(id) == 8:
                print('complete id',id)
                # scoreboard.add_UID(id)
                # scoreboard.getCurrentScore()
                id = ''

def reconnect():
    print('reconnecting, please ignore the error')
    ok = bt.do_connect(bt.port)
    if ok:
        print('connected, opening')
        while not bt.is_open(): pass
        print('opened')
        return True
    else:
        print('fail to connect')
        return False

def write():
    while True:
        msgWrite = input("type to control your car, or type E/R to exit/reconnect: ")
        
        if msgWrite == "E": sys.exit()
        if msgWrite == "R": 
            reconnect()
            continue
        try:
            bt.write(msgWrite + "\n")
        except:
            print('fail to write')

if __name__ == "__main__":
    # Please modify the port name.
    firstTry = True
    while firstTry:
        try:
            bt = bluetooth("COM6")
            firstTry = False
        except:
            time.sleep(1)
            print('reconnecting')
    while not bt.is_open(): pass
    print("BT Connected!")


    readThread = threading.Thread(target=read)
    readThread.setDaemon(True)
    readThread.start()

    writeThread = threading.Thread(target=write)
    writeThread.setDaemon(True)
    writeThread.start()

    # msgWrite = findPath()
    # print(msgWrite)
    # bt.write(msgWrite)
    while True:
        pass