import os
from cppy import cpon
from cppy import cspace

if __name__ == '__main__' : 
    trainer = cpon()
    trainer.setTrRatio(0.5)

    for dirpath, dirnames, filenames in os.walk('.\data') : 
        for filename in filenames : 
            filepath = os.path.join(dirpath, filename)

            with open(filepath) as src : 
                name = filename[:-4]
                srcdata = []
                for line in src.readlines() : 
                    line = line.split(',')
                    row = [float(dim) for dim in line]
                    srcdata.append(row)

            cs = trainer.csfactory(name, srcdata)
            trainer.appendcs(cs)

    trainer.train()
    res, resname = trainer.test()
    for ult in res : 
        print ult
    for ultname in resname : 
        print ultname
    pass
