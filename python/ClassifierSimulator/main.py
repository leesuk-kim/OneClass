__author__ = 'lk'

from sklearn import datasets
# from cppy import cpon, cspace
# from lkpy import lkexporter as xprt
import NIPSimulator
from pprint import pprint


if __name__ == '__main__':

    cm = NIPSimulator.ClfSim(fold=10)  # default : 2
    # cm.addclf(NIPSimulator.clffactory('svm'))
    # cm.addclf(NIPSimulator.clffactory('knn'))
    cm.addclf(NIPSimulator.clffactory('cpon'))

    iris = datasets.load_iris()

    cm.fit(iris.data[:, :2], iris.target)

    pstlist = cm.learn()

    for pst, sim in list(zip(pstlist, cm.simtaglist)):
        a = pst.statistics
        b = pst.predlist
        print(sim.simulorname + 'measurement')
        pprint(a)
        print(sim.simulorname + 'predict board')
        print(*b, sep='\n')

    print("End py")
