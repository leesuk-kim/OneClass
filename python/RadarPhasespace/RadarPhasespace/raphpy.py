# -*- coding: cp949 -*-
import re
import matplotlib.pyplot as plt
import multiprocessing as mp
import os

fttlist = ["NO", " S", " V", " AOA", " FREQ", " B", " FMOP", " AMP", " TOA", " PMOP", " PW", " dTOA"]
class raph:

    def __init__(self, classname): 
        self.name = classname
        '''emitter name'''
        self.srclist = []
        '''original data'''
        self.appendable = True
        '''
        flag for whether this class is appendable.
        if not appendable, appending functions don't work.
        '''
        pass
    
    def append_data(self, dlist): 
        '''샘플마다 읽어들여서 해당 클래스에 붙인다.
        '''
        if self.appendable:     
            srcbfr = []
            phbfr = []
            for d in dlist: 
                s = re.sub(' +', ' ', d)#change 1 more whitespaces to a whitespace in the string d
                s = s[1:].split(' ')
                s = [float(x) for x in s]
                srcbfr.append(s)#NO, S, V, AOA, FREQ, B, FMOP, AMP, TOA, PMOP, PW, dTOA
        
            #srcbfr = zip(*srcbfr)#어디서하나 똑같은데 close가 좀 허전하니까..

            self.srclist.append(srcbfr)
        pass

    def close(self): 
        self.appendable = False

        for i, v in enumerate(self.srclist): 
            self.srclist[i] = list(zip(*v))#transpose matrix!!!!!!
        pass

    pass


def statfeatures(arr, moments: str = "mvsk"):
    """
    moment - m/v/s/k. passive is mv.
    if mv, it returns m and v, or if mvk, it returns m, v, and k.
    """
    sfarr = []
    ll = float(len(arr))
    lls = ll - 1
    dll,  dlls = 1 / ll, 1 / lls

    if 'm' in moments: 
        m = sum(arr) / ll
        sfarr.append(m)
        
    if 'v' in moments : 
        v = sum([dlls * (x - m) ** 2 for x in arr])
        sfarr.append(v)

    if 's' in moments : 
        m3 = (dll * sum([dlls * (x - m) ** 3 for x in arr])) 
        b1 = m3 / (v ** 1.5)
        sfarr.append(b1)

    if 'k' in moments : 
        vsquare = (sum([dlls * (x - m) ** 2 for x in arr])) ** 2
        m4 = (dll * sum([dlls * (x - m) ** 4 for x in arr]))
        g2 = m4 / vsquare - 3
        sfarr.append(g2)

    return sfarr


def featurescaling(clist: list) :
    """AMP는 올바르게 작동하지 않습니다.
    """
    '''
    cslist : class sample list
    dslist : dimensional sample list 
    cdslist : list for dimensional sample of classes
    slist : sample list
    clist : class list
    dxlist : list for max of dimension
    dnlist : list for min of dimension 
    dflist : list for feature on dimension
    '''
    if type(clist[0]) is not raph : 
        print("TYPE INCORRECT")
        return 0

    cdslist = [list(zip(*cls.srclist)) for cls in clist] # 각 class마다 시간순으로 정리된 sample을 dimension으로 정리해서 복사
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

def plotclsphsp(rph: raph) :
    print("plotting", rph.name)
    dirname = os.path.join(os.path.dirname(__file__), rph.name)
    if not os.path.exists(dirname) : 
        os.makedirs(dirname)

    xps, yps = [], []
    dsflist = list(zip(*rph.srclist))  # dim x smpl x ftr
    for sflist, ftt in list(zip(dsflist, fttlist)) : 
        ftitle = 'PhaseSpace_'+ftt+'_'+rph.name
        fig = plt.figure(ftitle)
        plt.title(ftitle)
        
        for flist in sflist : 
            xps, yps = flist[:-1], flist[1:]
            plt.plot(xps, yps, 'k.')
        filename = os.path.join(dirname, ftitle)
        plt.savefig(filename)
        plt.clf()  # 여기서 close 안 해주면 자꾸 뒈짐 ㄷㄷ
        pass
    pass

def plotclshistogram(rph: raph) :
    print("histogarmming", rph.name)

    dirname = os.path.join(os.path.dirname(__file__), rph.name)
    if not os.path.exists(dirname) : 
        os.makedirs(dirname)

    dsflist = list(zip(*rph.srclist))  # dim x smpl x ftr
    for sflist, ftt in list(zip(dsflist, fttlist)) : 
        ftitle = 'HIstogram_'+ftt+'_'+rph.name
        fig = plt.figure(ftitle)
        plt.title(ftitle)
        
        histo = []
        for flist in sflist : 
            histo.extend(flist)
        plt.hist(histo, 10, normed=1, histtype='bar')
        
        filename = os.path.join(dirname, ftitle)
        plt.savefig(filename)
        plt.clf()  # 여기서 close 안 해주면 자꾸 뒈짐 ㄷㄷ
        pass
    pass