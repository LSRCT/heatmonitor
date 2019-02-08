import serial
import time
import os.path
import matplotlib.pyplot as plt

# saves dict to csv using keys as headers
def saveToCSV(savedict, filename = "", path = ""):
    if not filename:
        datanum = 1
        filename = "data"
        while os.path.isfile("".join([filename,str(datanum),".csv"])):
            datanum += 1
        filename = "".join([filename,str(datanum)])
    if filename[-4:0] != ".csv":
        filename = "".join([filename,".csv"])
    if not os.path.isfile(filename):
        file = open("".join([path,filename]),'w')
        file.write("".join([";".join(list(savedict.keys())),";\n"]))
        file.close()
    file = open("".join([path,filename]),'a')
    valueslist = list(savedict.values())
    for rowcount in range(len(valueslist[0])):
        for values in valueslist:
            file.write("".join([str(values[rowcount]),";"]))
        file.write(";\n")
    return 1

def readData(filename, path = ""):
    data = dict()
    file = open("".join([path, filename]),'r')
    keyline = file.readline().strip('\n').split(";")
    for key in keyline:
        print(key)
        data[key] = []
    line = list(map(float,(file.readline().strip('\n').split(";"))))
    while len(line)>1:
        for pos in range(0,len(keyline)):
            data[keyline[pos]].append(line[pos])
        print(line)
        try: line = list(map(float,(file.readline().strip('\n').split(";"))))
        except: line = []
    file.close()
    return data


if __name__ == "__main__":
    testdict = {"temperature": [1,2,3,4], "length":[4,5,6,7], "time":[8,9,10,11]}
    saveToCSV(testdict)
    # data = readData("data2.csv")
    # print(data)
    # plt.plot(data["Timestamp[h]"],data["Temp[°C]"])
    # plt.show()
