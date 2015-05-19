import os
from cppy import cpon
from cppy import cspace

if __name__ == '__main__' : 
    trainer = cpon(10)

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
        fn = 'fold#%02d.csv' % idx
        with open(os.path.join(path, fn), 'w') as f : 
            f.write('target, expect\n')
            for tci, tc in enumerate(fold) : 
                dl = len(tc)
                for i, data in enumerate(tc) : 
                    f.write('ep%02d,ep%02d\n'%(tci+1, data+1))
            pass
    pass
