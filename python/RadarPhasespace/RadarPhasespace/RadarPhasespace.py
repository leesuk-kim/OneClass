# -*- coding: cp949 -*-

import sys
import os
from raphpy import raph
import raphpy as rph
import multiprocessing as mp

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
                rre = raph('_'.join(clsname[2:]))
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
                print('append %s' % rre.name)
    rph.featurescaling(rrelist)#for feature scaling

    pool = mp.Pool()
    pool.map(rph.plotclsphsp, rrelist)

    #for rre in rrelist : 
    #    rph.plotclsphsp(rre)

    """
    1. data를 읽는다.
    2. feature를 추려낸다.
    3. phasespace(x_K ~ x_{K+1})와 histogram을 그린다.
    4. 같은 class의 sample을 모두 합쳐서 phasespace(x_K ~ x_{K+1})와 histogram을 그린다.
    """