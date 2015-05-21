 import os
from cppy import cpon
from cppy import cspace
import time
if __name__ == '__main__' : 
    trainer = cpon(10)

    #for dirpath, dirnames, filenames in os.walk('.\data12') : 
    #for dirpath, dirnames, filenames in os.walk('.\data24') : 
    for dirpath, dirnames, filenames in os.walk('.\data') : 
        for filename in filenames : 
            filepath = os.path.join(dirpath, filename)

            with open(filepath) as src : 
                name = filename[:-4]
                print 'upload ' + name
                srcdata = []
                for line in src.readlines() : 
                    line = line.split(',')
                    row = [float(dim) for dim in line]
                    srcdata.append(row)

            cs = trainer.csfactory(name, srcdata)
            trainer.registcs(cs)

    report = trainer.Learn()
    
    path = os.path.join(os.path.dirname(__file__), 'report')
    for idx, fold in enumerate(report) : 
        fn = 'raw#%02d.csv' % idx
        with open(os.path.join(path, fn), 'w') as f : 
            f.write('target,expect\n')
            for tci, tc in enumerate(fold) : 
                dl = len(tc)
                for i, data in enumerate(tc) : 
                    f.write('ep%02d,ep%02d\n'%(tci+1, data+1))

    path = os.path.join(os.path.dirname(__file__), 'report')
    for idx, fold in enumerate(report) : 
        fn = 'board#%02d.csv' % idx
        fl = len(fold)
        board = [[0 for y in range(fl)] for x in range(fl)]
        for i, ec in enumerate(fold) : 
            for j, data in enumerate(ec) : 
                board[i][data] += 1
        with open(os.path.join(path, fn), 'w') as f : 
            f.write('target,')
            for i in range(len(board)) : 
                f.write('ep%02d%c' %(i + 1, ',' if len(board) - i != 1 else '\n'))
            for i, row in enumerate(board) : 
                f.write('ep%02d,' %(i + 1))
                for j, d in enumerate(row) : 
                    f.write('%02d%c' %(d, ',' if len(row) - j != 1 else '\n'))
            pass

    #report = trainer.learnSVM()
    pass
