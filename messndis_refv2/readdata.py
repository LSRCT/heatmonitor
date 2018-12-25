import serial
import time
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

def readSerial():
    stopcounter = 0
    valuearray = []
    ser = serial.Serial('COM5', 9600, timeout=1)
    time.sleep(2)
    ser.write(1)
    while 1:
        if (ser.inWaiting()>0):
            readvalue = int(ser.readline().decode('utf-8').rstrip())
            valuearray.append(readvalue)
            stopcounter += 1
        if stopcounter == 1024:
            return valuearray
            break
if __name__ == "__main__":
    data = readSerial()
    plt.plot(range(len(data)),data)
    plt.show()
