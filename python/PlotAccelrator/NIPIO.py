__author__ = 'lk'

import xlsxwriter as writer
import os
import time
from mysql import connector

directory = "D:\\Document\\niplab\\LIG"
version = "\\july"
trails = "\\output\\"
# dataname = "oneten_TOA"
# dataname = "originalten_TOA"
# dataname = "july_original_TOA"
dataname = "july_originbeam_TOA"
timestamp = time.time()


def load_data():
    path = 'D:\\Document\\niplab\\LIG\\july\\source\\1000\\inputdata'
    with open(os.path.join(path, dataname + '.csv'), 'r') as merge:
        data, target = [], []
        for line in merge.readlines():
            line = line.split(',')
            tag, datum = line[0], [float(x) for x in line[1:]]
            target.append(tag)
            data.append(datum)

    return data, target


def import_data():
    data, target = [], []
    conn = connector.connect(host='221.153.219.220', port='8806', user='niplab', password='qwqw`12', database='LIG')
    cursor = conn.cursor()
    # query = "SELECT * from july_originbeam_dTOA"
    query = "SELECT * from " + dataname + " WHERE name LIKE \'E0079%\' OR name LIKE \'E0080%\'"
    cursor.execute(query)
    for feature in cursor:
        data.append(feature[2:])
        target.append(feature[1])
    return data, target