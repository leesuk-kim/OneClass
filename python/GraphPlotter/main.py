__author__ = 'lk'


import os
from GraphPlot import DIO


source = "D:\\Document\\niplab\\LIG\\july\\source\\1000\\inputdata\\inputdata_oneten"
destination = "D:\\Document\\niplab\\LIG\\july\\destination\\1000\\origin"

if __name__ == "__main__":
    for dirname, dirnames, filenames in os.walk(source):
        fns = filenames[:-1]
        for fn in fns:
            with open(source + "\\" + fn) as f:
                data = [[float(y) for y in x.split(sep=',')] for x in f.readlines()]
                dio = DIO(fn[:-4], data)
                dio.histogram(destination)
                a = 1
