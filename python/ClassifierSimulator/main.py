__author__ = 'lk'

# from cppy import cpon, cspace
# from lkpy import lkexporter as xprt
import NIPSimulator
import NIPIO


if __name__ == '__main__':
    # cm = NIPSimulator.ClfSim()  # default : 2
    cm = NIPSimulator.ClfSim(fold=10)  # default : 2
    # cm.addclf(NIPSimulator.clffactory('svm'))
    # cm.addclf(NIPSimulator.clffactory('knn'))
    cpon = NIPSimulator.clffactory('cpon', cluster='lk', beta='scipy', bse='mm', threadable=False)
    cm.addclf(cpon)

    # data, target = NIPIO.load_data()
    data, target = NIPIO.import_data()
    # _data, _target = [], []
    # for d, t in zip(data, target):
    #     if(t == '05' or t == '42'):
    #         _data.append(d)
    #         _target.append(t)
    # data, target = _data, _target

    cm.unknown = True
    cm.fit(data, target)
    # cm.fit_unknown(data, target, ratio=0.1)

    pstlist = cm.learn()

    NIPIO.measurement(pstlist)
    for cponpst in pstlist:
        if 'cpon' in cponpst.simulorname:
            NIPIO.p_value(cponpst)
            # print(cponpst.simulor.pred_pval)
            # for i, ppv_fold in enumerate(cponpst.simulor.pred_pval):
            #     print(("fold %02d" % i) + "p-value result")
            #     ppvstr = ""
            #     for ppv_cls in ppv_fold:
            #         ppvstr += ppv_cls + "\t" + repr(ppv_fold[ppv_cls]) + "\t"
            #     print(ppvstr)

    print("End py")
