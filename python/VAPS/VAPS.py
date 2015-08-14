__author__ = 'lk'

import re
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import numpy
import os

fttlist = ["NO", " S", " V", " AOA", " FREQ", " B", " FMOP", " AMP", " TOA", " PMOP", " PW", " dTOA"]


def velocite(src):
    dimlist = []
    for dim in src:
        v1 = dim[:-1]
        v2 = dim[1:]
        v = [b - a for a, b in zip(v1, v2)]
        dimlist.append(v)

    return dimlist


def accelerate(src):
    dimlist = []
    for dim in src:
        acc1 = dim[:-2]
        acc2 = dim[1:-1]
        acc3 = dim[2:]
        v = [2 * b - a - c for a, b, c in zip(acc1, acc2, acc3)]
        dimlist.append(v)

    return dimlist


class VAPS:
    target = ["NO", " S", " V", " AOA", " FREQ", " B", " FMOP", " AMP", " TOA", " PMOP", " PW", " dTOA"]

    def __init__(self, classname):
        self.name = classname
        """emitter name"""
        self.srclist = []
        """original data"""
        self.appendable = True
        """
        flag for whether this class is appendable.
        if not appendable, appending functions don't work.
        """
        self.velocity_list = []
        self.acceleration_list = []
        pass

    def append_data(self, dlist):
        """샘플마다 읽어들여서 해당 클래스에 붙인다.
        """
        if self.appendable:
            srcbfr = []
            phbfr = []
            for d in dlist:
                s = re.sub(' +', ' ', d)  # change 1 more whitespaces to a whitespace in the string d
                s = s[1:].split(' ')
                s = [float(x) for x in s]
                srcbfr.append(s)  # NO, S, V, AOA, FREQ, B, FMOP, AMP, TOA, PMOP, PW, dTOA

            # srcbfr = zip(*srcbfr)# 어디서하나 똑같은데 close가 좀 허전하니까..

            self.srclist.append(srcbfr)
        pass

    def close(self):
        self.appendable = False

        for i, v in enumerate(self.srclist):
            self.srclist[i] = list(zip(*v))  # transpose matrix!!!!!!
        pass

    def append_va(self):
        for i, v in enumerate(self.srclist):
            self.velocity_list.append(velocite(self.srclist[i]))
            self.acceleration_list.append(accelerate(self.srclist[i]))

    pass


def histogram(a: list, bins=10):
    an, ax = min(a), max(a)
    hist = [0 for x in range(bins)]
    a.sort()
    step = (ax - an) / bins
    bound = an + step
    i = 0
    for x in a:
        if x > bound:
            bound += step
            i += 1
        hist[i] += 1
    return hist


def statfeatures(arr, moments: str="mvsk"):
    """
    moment - m/v/s/k. passive is mv.
    if mv, it returns m and v, or if mvk, it returns m, v, and k.
    """
    sfarr = []
    ll = float(len(arr))
    lls = ll - 1
    dll,  dlls = 1 / ll, 1 / lls
    v, m = 0., 0.

    if 'm' in moments:
        m = sum(arr) / ll
        sfarr.append(m)

    if 'v' in moments:
        v = sum([dlls * (x - m) ** 2 for x in arr])
        sfarr.append(v)

    if 's' in moments:
        m3 = (dll * sum([dlls * (x - m) ** 3 for x in arr]))
        b1 = m3 / (v ** 1.5)
        sfarr.append(b1)

    if 'k' in moments:
        vsquare = (sum([dlls * (x - m) ** 2 for x in arr])) ** 2
        m4 = (dll * sum([dlls * (x - m) ** 4 for x in arr]))
        g2 = m4 / vsquare - 3
        sfarr.append(g2)

    return sfarr


def featurescaling(clist: list):
    """AMP는 올바르게 작동하지 않습니다.
    """
    """
    cslist: class sample list
    dslist: dimensional sample list
    cdslist: list for dimensional sample of classes
    slist: sample list
    clist: class list
    dxlist: list for max of dimension
    dnlist: list for min of dimension
    dflist: list for feature on dimension
    """
    if type(clist[0]) is not VAPS:
        print("TYPE INCORRECT")
        return 0

    cdslist = [list(zip(*cls.srclist)) for cls in clist]  # 각 class마다 시간순으로 정리된 sample을 dimension으로 정리해서 복사
    dslist = list(zip(*cdslist))  # 각 class순으로 저장된 list를 dimension으로 정리해서 복사
    dxlist = [max([max([max(z) for z in y]) for y in x]) for x in dslist]  # 각 dimension의 최대값
    dnlist = [min([min([min(z) for z in y]) for y in x]) for x in dslist]  # 각 dimension의 최소값
    ddlist = [dx - dn for dx, dn in zip(dxlist, dnlist)]

    for cls in clist:  # each class
        fsrclist = []
        for slist in cls.srclist:  # each sample
            fslist = []
            for dflist, dn, dd in zip(slist, dnlist, ddlist):  # each list for feature of dim, dn, dd
                fdflist = [0 if df == 0 or dd == 0 or df == dn else (df - dn) / dd for df in dflist]
                fslist.append(fdflist)
                pass
            fsrclist.append(fslist)
            pass
        cls.srclist = fsrclist
        pass
    pass


def plotgraph(name, data, datatype: '', outputpath):
    print(datatype, name)
    dirname = os.path.join(outputpath, name)
    if not os.path.exists(dirname):
        os.makedirs(dirname)

    # xps, yps = [], []
    dsflist = list(zip(*data))  # dim x smpl x ftr
    for sflist, ftt in list(zip(dsflist, fttlist)):
        a = []
        for x in sflist:
            a.extend(x)
        a.sort()
        # an, ax = min(a), max(a)
        # if ax - an == 0:
        #     continue
        # a = [(x - an)/(ax - an) for x in a]

        # h = histogram(a, bins=15)
        h, bins = numpy.histogram(a, bins=50)
        s = numpy.std(a, ddof=1)
        m = numpy.mean(a)
        # if s != 0:
        ftitle = datatype + '_' + ftt + '_' + name
        fig = plt.figure(ftitle)
        plt.title(ftitle)
        pdf = mlab.normpdf(bins, m, s)

        plt.plot(bins, pdf, 'k-')
        filename = os.path.join(dirname, ftitle)
        plt.savefig(filename)
        plt.close(fig)
        pass
    pass


def plotclsphsp(rph: VAPS, outputpath):
    print("plotting", rph.name)
    dirname = os.path.join(outputpath, rph.name)
    if not os.path.exists(dirname):
        os.makedirs(dirname)

    # xps, yps = [], []
    dsflist = list(zip(*rph.srclist))  # dim x smpl x ftr
    for sflist, ftt in list(zip(dsflist, fttlist)):
        ftitle = 'PhaseSpace_'+ftt+'_'+rph.name
        fig = plt.figure(ftitle)
        plt.title(ftitle)

        for flist in sflist:
            xps, yps = flist[:-1], flist[1:]
            plt.plot(xps, yps, 'k.')
        filename = os.path.join(dirname, ftitle)
        plt.savefig(filename)
        plt.close(fig)
        pass
    pass


def plotclshistogram(rph: VAPS, outputpath):
    print("histogarmming", rph.name)

    dirname = os.path.join(outputpath, rph.name)
    if not os.path.exists(dirname):
        os.makedirs(dirname)

    dsflist = list(zip(*rph.srclist))  # dim x smpl x ftr
    for sflist, ftt in list(zip(dsflist, fttlist)):
        ftitle = 'HIstogram_' + ftt + '_' + rph.name
        fig = plt.figure(ftitle)
        plt.title(ftitle)

        histo = []
        for flist in sflist:
            histo.extend(flist)
        plt.hist(histo, 10, normed=1, histtype='bar')

        filename = os.path.join(dirname, ftitle)
        plt.savefig(filename)
        plt.close(fig)
        pass
    pass


def plotcmpgraph(*args, **kargs):
    datalist = args[0]
    k = kargs
    name = kargs['name']
    outputdir = kargs['outputdir']
    target = kargs['target']
    datalist = [list(zip(*x)) for x in datalist]
    datalist = list(zip(*datalist))
    totaldata = []
    for targetdata in datalist:
        타겟데이터 = []
        for objdata in targetdata:
            data = []
            for d in objdata:
                data.extend(d)
            타겟데이터.append(data)
        totaldata.append(타겟데이터)

    for targetdata, tg in zip(totaldata, target):
        ftitle = name + '_' + tg
        fig = plt.figure(ftitle)
        plt.title(ftitle)
        legends = []
        for objdata, clsname in zip(targetdata, ['79', '80']):
            h, bins = numpy.histogram(objdata, bins=50)
            s = numpy.std(objdata, ddof=1)
            m = numpy.mean(objdata)
            pdf = mlab.normpdf(bins, m, s)

            l = plt.plot(bins, pdf, label=clsname)
            legends.append(l)
        plt.legend()
        filename = outputdir + '/' + ftitle
        plt.savefig(filename)
        plt.close(fig)
    return k
