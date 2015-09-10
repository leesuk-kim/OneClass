__author__ = 'lk'

# from cppy import cpon, cspace
# from lkpy import lkexporter as xprt
import NIPSimulator
import NIPIO


if __name__ == '__main__':
    cm = NIPSimulator.ClfSim()  # default : 2
    # cm = NIPSimulator.ClfSim(fold=10)  # default : 2
    # cm.addclf(NIPSimulator.clffactory('svm'))
    # cm.addclf(NIPSimulator.clffactory('knn'))
    cm.addclf(NIPSimulator.clffactory('cpon'))

    # data, target = NIPIO.load_data()
    data, target = NIPIO.import_data()

    # cm.fit(iris.data[:, :2], iris.target)
    cm.fit(data, target)

    pstlist = cm.learn()

    NIPIO.measurement(pstlist)
    cponpstlist = [x for x in pstlist if 'cpon' in x.simulorname]
    for cponpst in cponpstlist:
        NIPIO.p_value(cponpst)
    # 한글을 살려라!
    print("End py")
