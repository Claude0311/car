import threading
import time
import sys
import serial
import score


class bluetooth:
    def __init__(self, port: str, baudrate: int=9600):
        """ Initialize an BT object, and auto-connect it. """
        # The port name is the name shown in control panel
        # And the baudrate is the communication setting, default value of HC-05 is 9600.
        self.ser = serial.Serial(port, baudrate=baudrate)
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
        # Write the byte to the output buffer, encoded by ascii.
        send = output.encode("utf-8")
        self.ser.write(send)

    def readString(self) -> str:
        # Scan the input buffer until meet a '\n'. return none if doesn't exist.
        if(self.waiting()):
            receiveMsg = self.ser.readline().decode("utf-8")[:-1]

        return receiveMsg

# def read(bt):
#     while True:
#         time.sleep(0.01)
#         if bt.waiting():
#             print(bt.readString())

def read_RFID(bt, RFIDs, dir):
    scoreboard = score.Scoreboard(dir)
    id = ''
    while True:
        time.sleep(0.01)
        if bt.waiting():
            temp = bt.readString()
            print(temp)
            id += temp if len(temp) == 2 else ('0' + temp)
            if len(id) == 8:
                RFIDs.add(id)
                scoreboard.add_UID(id)
                scoreboard.getCurrentScore()
                print(RFIDs)
                id = ''

def write(bt):
    while True:
        msgWrite = input()
        if msgWrite == "exit": sys.exit()
        bt.write(msgWrite + "\n")

if __name__ == "__main__":
    # Please modify the port name.
    bt = bluetooth("COM5")
    while not bt.is_open(): pass
    print("BT Connected!")

    readThread = threading.Thread(target=read, args=(bt,))
    readThread.setDaemon(True)
    readThread.start()

    while True:
        msgWrite = input()
        if msgWrite == "exit": sys.exit()
        bt.write(msgWrite)
