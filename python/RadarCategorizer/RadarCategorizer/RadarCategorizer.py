#  -*- coding: cp949 -*-
import os, time
import sys
from cppy import cpon, cspace
from lkpy import lkexporter as xprt
import clfmgmt
from sklearn import datasets

if __name__ == '__main__':
    cm = clfmgmt.clfmgr(fold=10)
    cm.addclf(clfmgmt.clffactory('svm'))
    iris = datasets.load_iris()

    cm.uploadres(iris.data, iris.target)
    a = cm.folding()
    b = next(a)
    b = next(a)
    b = next(a)
    b = next(a)
    trainer = cpon(10)
    xprttrainer = xprt(trainer)
    # src = 'data'
    # src = 'data_06_fs'
    # src = 'data_12'
    # src = 'data_12_fs'
    # src = 'd5_12fs'
    # src = 'data_24'
    # src = 'data_24_fs'
    # src = 'data_24_ddtoa'
    # src = 'data_abs_24_ddtoa'
    # src = 'lig_12'
    src = 'data_eb'
    trainer._srcdir = src
    for dirpath, dirnames, filenames in os.walk(src): 
        for filename in filenames: 
            filepath = os.path.join(dirpath, filename)

            with open(filepath) as src: 
                name = filename[:-4]
                print('load ' + name)
                srcdata = []
                for line in src.readlines(): 
                    line = line.split(',')
                    row = [float(dim) for dim in line]
                    srcdata.append(row)

            cs = trainer.csfactory(name, srcdata)
            trainer.registcs(cs)

    trainer.learn()
    xprttrainer.xlsxctrdcmap()
    xprttrainer.xlsxclfscore()
    xprttrainer.xlsxclfboard()
    # trainer.learnSVM()
    # trainer.learnKNN()
    # xprttrainer.csvaprf()
    xprttrainer.xlsxparfe()
    xprttrainer.xlsxtfpnaprf()
    # xprttrainer.xslxsklearn()
    # xprttrainer.xlsxknnsb()
    # report = trainer.learnSVM()
    # print trainer._knnscoreboard
    pass
