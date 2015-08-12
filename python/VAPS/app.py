__author__ = 'lk'

import os

trail = 'D:\\Document\\niplab\\LIG\\july\\source\\1000\\rawdata_oneten'

if __name__ == '__main__':
    for dirname, dirnames, filenames in os.walk(trail):
        if len(filenames) > 0:
            a = filenames
            