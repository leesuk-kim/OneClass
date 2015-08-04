# -*- coding:CP949 -*-
__author__ = 'lk'

from sklearn import datasets
# from cppy import cpon, cspace
# from lkpy import lkexporter as xprt
import NIPSimulator


if __name__ == '__main__':

    cm = NIPSimulator.ClfSim(fold=10)  # default : 2
    cm.addclf(NIPSimulator.clffactory('svm'))
    cm.addclf(NIPSimulator.clffactory('knn'))

    iris = datasets.load_iris()

    cm.fit(iris.data, iris.target)

    pstlist = cm.learn()

    for pst in pstlist:
        a = pst.statistics
        b = pst.predlist

    print("End py")
