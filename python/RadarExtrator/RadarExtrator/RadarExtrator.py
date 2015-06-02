import sys
import os
from raexpy import raex
import raexpy as rex
if __name__ == "__main__" : 
    rrelist = []
    for dirname, dirnames, filenames in os.walk('.'):
        # print path to all subdirectories first.
        for subdirname in dirnames:
            os.path.join(dirname, subdirname)
            #print(os.path.join(dirname, subdirname))

        if 'data' in dirname :
            if filenames : 
                clsname = dirname.split('\\')
                rre = raex('_'.join(clsname[2:]))
                # print path to all filenames in subdirname.
                for filename in filenames:
                    if '.txt' in filename and not '%' in filename:
                        os.path.join(dirname, filename)
                        #print os.path.join(dirname, filename)
                        dlist = []
                        file = open(dirname + '\\' + filename, 'r')
                        file.readline(), file.readline(), file.readline()
                        lines = file.readlines()
                        for line in lines : 
                            dlist.append(line)
                        rre.appendData(dlist)
                rre.close()
                #print data
                rrelist.append(rre)
                print 'append %s' % rre.getName()
    rex.featurescaling(rrelist)#for feature scaling
    #export Data
    with open('merge.csv', 'wb') as fm : 
        for c in rrelist : 
            with open(c.getName() + '.csv', 'wb') as f : 
                for data in c.getRawData() : 
                    dlen = len(data) - 1
                    for i, val in enumerate(data) : 
                        f.write('%s%c' % (repr(val), ',' if dlen != i else '\n'))
                        fm.write('%s%c' % (repr(val), ',' if dlen != i else '\n'))

                print 'file write : ' + c.getName() + '.csv'