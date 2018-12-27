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

def saveData(data):
    if data:
        file = open('heatdata.csv','a')
        file.write('Heatdata,'+',\n')
        file.write('Number,Data,Timestamp'+"\n")
        for i in range(len(data)):
            file.write(str(i)+','+str(data[i])+','+str(((float(i)*1000*(100+50)))/(1000*60*60))+"\n")
        file.close()

if __name__ == "__main__":
    data = readSerial()
    saveData(data)
    plt.plot(range(len(data)),data)
    plt.show()
