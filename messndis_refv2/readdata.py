import serial
import serial.tools.list_ports
import time
import matplotlib.pyplot as plt
import dataToolBox as dTB

def readSerial():
    myports = [tuple(p) for p in list(serial.tools.list_ports.comports())]
    port = myports[0][0]
    stopcounter = 0
    valuearray = []
    ser = serial.Serial(port, 9600, timeout=10)
    time.sleep(2)
    ser.write(1)
    while len(valuearray) < 1024:
        if (ser.inWaiting()>0):
            readvalue = int(ser.readline().decode('utf-8').rstrip())
            valuearray.append(readvalue)
    return valuearray

def correctStamps(data):
    newdata = [[],[]]
    try: startindex = data.index(250)+1
    except: startindex = 0; print("250 not found")
    if startindex == len(data):
        startindex = 0
    for i in range(len(data)-1):
        dataindex = i+startindex
        if dataindex>len(data)-1:
            dataindex -= len(data)
        newdata[0].append(((float(i)*1000*(100+50)))/(1000*60*60))
        newdata[1].append(data[dataindex])
    return newdata

if __name__ == "__main__":
    data = {"Timestamp[h]": [], "Temp[°C]": []}
    raw = readSerial()
    raw = correctStamps(raw)
    data["Timestamp[h]"] = raw[0]
    data["Temp[°C]"] = raw[1]
    dTB.saveToCSV(data)
