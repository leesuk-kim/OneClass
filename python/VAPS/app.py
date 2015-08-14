__author__ = 'lk'

import os
from vaps import VAPS
import vaps
import multiprocessing
import threading

# trail = 'D:\\Document\\niplab\\LIG\\july\\source\\1000\\rawdata_oneten'
trail = 'D:/Document/niplab/LIG/'
season = 'july/'
# source = 'source/1000/rawdata'
# source = 'source/1000/rawdata_oneten'
source = 'source/1000/rawdata_7980_oneten'
output = trail + season + 'VAPS/7980_oneten'


def walker(*args):
    dirname, dirnames, filenames = args[0][0], args[0][1], args[0][2]
    clsname = os.path.split(dirname)

    if len(filenames) < 1:
        return 0

    phsp = VAPS(clsname[-1])
    for filename in filenames:
        if 'txt' in filename:
            filepath = os.path.join(dirname, filename)

            with open(filepath, 'r') as file:
                file.readline(), file.readline(), file.readline()
                phsp.append_data(file.readlines())
    phsp.close()
    phsp.append_va()
    # vaps.plotgraph(phsp.name, phsp.velocity_list, 'velocity', output)
    # vaps.plotgraph(phsp.name, phsp.acceleration_list, 'acceleration', output)
    # vaps.plotcmpgraph(phsp.)
    # vaps.plotclsphsp(phsp, output)
    # vaps.plotclshistogram(phsp, output)
    return phsp

if __name__ == "__main__":
    #pool = multiprocessing.Pool()
    #pool.map(walker, os.walk(trail + season + source))
    phsplist = [walker(x) for x in os.walk(trail + season + source)]
    vaps.plotcmpgraph([p.velocity_list for p in phsplist if type(p) == VAPS], name='velocity', outputdir=output, target = VAPS.target)
    vaps.plotcmpgraph([p.acceleration_list for p in phsplist if type(p) == VAPS], name='acceleration', outputdir=output, target = VAPS.target)
    print("fin")
