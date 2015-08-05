# -*- coding: cp949 -*-
import os
from raexpy import RadarExtractor
import raexpy as rex


if __name__ == "__main__": 
    rrelist = []
    for dirname, dirnames, filenames in os.walk('.'):
        # print path to all subdirectories first.
        for subdirname in dirnames:
            os.path.join(dirname, subdirname)
            # print(os.path.join(dirname, subdirname))

        if 'data' in dirname:
            if filenames: 
                clsname = dirname.split('\\')
                rre = RadarExtractor('_'.join(clsname[2:]))
                # print path to all filenames in subdirname.
                for filename in filenames:
                    if '.txt' in filename and '%' not in filename:
                        os.path.join(dirname, filename)
                        # print os.path.join(dirname, filename)
                        dlist = []
                        file = open(dirname + '\\' + filename, 'r')
                        file.readline(), file.readline(), file.readline()
                        lines = file.readlines()
                        for line in lines: 
                            dlist.append(line)
                        rre.append_data(dlist)
                rre.close()
                # print data
                rrelist.append(rre)
                print('append %s' % rre.name)
    rex.featurescaling(rrelist)  # for feature scaling
    # export Data
    dirname = os.path.dirname(__file__)
    dirname = os.path.join(dirname, "export")
    if not os.path.exists(dirname): 
        os.makedirs(dirname)

    with open(os.path.join(dirname, 'merge.csv'), 'wb') as fm: 
        for c in rrelist: 
            with open(os.path.join(dirname, c.name + '.csv'), 'wb') as f:
                for data in c.rawdata:
                    dlen = len(data) - 1
                    for i, val in enumerate(data): 
                        f.write(bytes('%s%c' % (repr(val), ',' if dlen != i else '\n'), 'cp949'))
                        fm.write(bytes('%s%c' % (repr(val), ',' if dlen != i else '\n'), 'cp949'))

                print('file write: ' + c.name + '.csv')
