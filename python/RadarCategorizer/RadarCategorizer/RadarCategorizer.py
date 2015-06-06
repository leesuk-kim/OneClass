import os, time
import sys
from cppy import cpon, cspace
from lkpy import lkexporter as xprt

if __name__ == '__main__' : 
    trainer = cpon(10)
    xprttrainer = xprt(trainer)
    #src = 'data'
    #src = 'data_06_fs'
    #src = 'data_03_fs'
    #src = 'data_12'
    src = 'data_12_fs'
    #src = 'data_24'
    #src = 'data_24_fs'
    #src = 'data_24_ddtoa'
    #src = 'data_abs_24_ddtoa'
    trainer._srcdir = src
    for dirpath, dirnames, filenames in os.walk(src) : 
        for filename in filenames : 
            filepath = os.path.join(dirpath, filename)

            with open(filepath) as src : 
                name = filename[:-4]
                print 'load ' + name
                srcdata = []
                for line in src.readlines() : 
                    line = line.split(',')
                    row = [float(dim) for dim in line]
                    srcdata.append(row)

            cs = trainer.csfactory(name, srcdata)
            trainer.registcs(cs)

    #trainer.learn()
    #xprttrainer.csvaprf()
    #xprttrainer.xlsxctrdcmap()
    #xprttrainer.xlsxclfscore()
    #xprttrainer.xlsxclfboard()
    #trainer.learnSVM()
    #xprttrainer.csvsklearn()
    trainer.learnKNN()
    xprttrainer.csvsklearn()
    xprttrainer.xlsxknnsb()
    #report = trainer.learnSVM()
    print trainer._knnscoreboard
    pass
