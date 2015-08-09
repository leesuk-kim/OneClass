# -*- coding: cp949 -*-

import os
from phasespace import PhaseSpace
import phasespace as rph
import multiprocessing
import threading

trail = 'D:/Document/niplab/LIG/'
season = 'july/'
# source = 'source/1000/rawdata'
source = 'source/1000/rawdata_oneten'
output = trail + season + 'PhaseSpace/100x10'


def walker(*args):
    dirname, dirnames, filenames = args[0][0], args[0][1], args[0][2]
    clsname = os.path.split(dirname)

    if len(filenames) < 1:
        return 0

    phsp = PhaseSpace(clsname[-1])
    for filename in filenames:
        if 'txt' in filename:
            filepath = os.path.join(dirname, filename)

            with open(filepath, 'r') as file:
                file.readline(), file.readline(), file.readline()
                phsp.append_data(file.readlines())

    rph.plotclsphsp(phsp, output)
    rph.plotclshistogram(phsp, output)

if __name__ == "__main__":
    pool = multiprocessing.Pool()
    pool.map(walker, os.walk(trail + season + source))
    print("fin")
