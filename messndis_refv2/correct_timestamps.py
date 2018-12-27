import time
import matplotlib
import matplotlib.pyplot as plt
import numpy as np



def readData(path):
    data = []
    file = open('heatdata.csv','r')
    line = file.readline()
    line = file.readline()
    line = file.readline()
    while line:
        data.append(line.split(",")[1])
        line = file.readline()
    file.close()
    return data

def correctStamps(data):
    file = open('heatdata_cs.csv','a')
    file.write('Heatdata,'+',\n')
    file.write('Number,Data,Timestamp'+"\n")
    startindex = data.index("250")+1
    if startindex == len(data):
        startindex = 0
    for i in range(len(data)):
        dataindex = i+startindex
        if dataindex>len(data)-1:
            dataindex -= len(data)
        file.write(str(i)+','+str(data[dataindex])+','+str(((float(i)*1000*(100+50)))/(1000*60*60))+"\n")
    file.close()


if __name__ == "__main__":
    path = "heatdata.csv"
    data = readData(path)
    print(data)
    print(len(data))
    correctStamps(data)
