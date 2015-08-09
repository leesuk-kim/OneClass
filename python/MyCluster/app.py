# -*- coding:CP949 -*-
from mycluster import MyCluster
from sklearn import datasets
import mycluster

__author__ = 'lk'
testdata = [[0.1,0.9],[0.1,0.9],[0.1,0.9],[0.1,0.9],[0.1,0.9],[0.1,0.9],[0.1,0.9],[0.1,0.9],
            [0.1,0.9],[0.1,0.9],[0.1,0.9],[0.2,0.8],[0.2,0.8],[0.2,0.8],[0.2,0.8],
            [0.2,0.8],[0.2,0.8],[0.2,0.8],[0.2,0.8],[0.3,0.7],[0.3,0.7],[0.3,0.7],[0.3,0.7],
            [0.3,0.7],[0.3,0.7],[0.4,0.6],[0.4,0.6],[0.4,0.6],[0.4,0.6],[0.5,0.5],
            [0.5,0.5],[0.6,0.4],[0.6,0.4],[0.6,0.4],[0.6,0.4],[0.7,0.3],[0.7,0.3],[0.7,0.3],
            [0.7,0.3],[0.7,0.3],[0.7,0.3],[0.8,0.2],[0.8,0.2],[0.8,0.2],[0.8,0.2],
            [0.8,0.2],[0.8,0.2],[0.8,0.2],[0.8,0.2],[0.9,0.1],[0.9,0.1],[0.9,0.1],[0.9,0.1],
            [0.9,0.1],[0.9,0.1],[0.9,0.1],[0.9,0.1],[0.9,0.1],[0.9,0.1],[0.9,0.1]]


if __name__ == '__main__':
    iris = datasets.load_iris()
    target, data = iris.target, iris.data

    targetset = set(target)
    for tag in targetset:
        clsdata = [x[1] for x in list(zip(target, data)) if tag == x[0]]
        mc = MyCluster(clsdata)
        # mc = MyCluster(mycluster.featurescaling(clsdata))
        # mc = MyCluster(testdata)
        cluster = mc.cluster()