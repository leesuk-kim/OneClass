# -*- coding: cp949 -*-
"""
Radar signal Categorizer
version 2.0
Designed by leesuk kim(aka. LK)
Lang. : Python
Lang. ver. : 3.4.3

"""
from functools import reduce
import math
import time
import numpy as np
import numpy.linalg as npla
from scipy.stats import beta
import scipy.stats as scistats
from sklearn.svm import SVC
from sklearn import metrics
from sklearn.neighbors import KNeighborsClassifier
import lkpy as lkep

class cpon : 
    '''
    Class Probability Output Network

    It has classes for classification.
    It has classifiers-SVM, kNN, and CPON.
    (DO NOT WRITE ON ENGLISH)

    '''
    
    def __init__(self, fold = 2, srcdir='') : 
        self._TimeStamp = '%d' % int(time.time())
        self._cslist = []
        self._fmax = fold
        self._kmax = 1
        self._cslist = []
        self._kcdboard = []
        self._parfelist = []
        '''
        a map consist of distance between each centroid of class
        '''
        self._clfboard = []
        self._srcdir = srcdir
        pass

    def csfactory(self, name = None, tmat = []) : 
        '''
        class space factory
        setting kernel volume from this class
        '''
        cs = cspace(name, tmat, self._fmax)
        return cs

    def registcs(self, cs) : 
        self._cslist.append(cs)

    def appendValidRow(self, row) : 
        #append test data
        pass
    def appendValid(self, matrix) : 
        try : 
            self.appendValidRow(matrix[x] for x in range(len(matrix)))
        except TypeError : 
            print('please input a vector.')

        pass

    def setFold(self, fold) : 
        self._fmax = fold

    def getcsnames(self) : 
        return [cs._name for cs in self._cslist]

    def setKnVol(self, knvol) : 
        '''if it has fixed number of kernel, then put number in.
        if grow is true, it makes kernel volmes most effective.
        '''
        self._kmax = knvol

    def learn(self) : 
        print('learning')
        clfboard, clfscoreboard = [], []
        self._clfAPRF = []
        self._parfelist = []
        self.predfolable = []
        self.yfolable = []
        for fold in range(self._fmax) : 
            lkep.fold = fold
            print('fold#%d' % fold)

            for cs in self._cslist : 
                cs.onFolding(fold)
                cs.onTraining()

            fkcdboard = mapctrd(self._cslist, self._fmax)
            self._kcdboard.append(fkcdboard)
            foldclfscore = scoreclf(self.onTesting(fold))
            clfscoreboard.append(foldclfscore)
            foldclf = boardclf(foldclfscore)
            clfboard.append(foldclf)
            parfelist, predtable, ytable = self.onOARTesting()
            self._parfelist.append(parfelist)
            self.predfolable.append(predtable)
            self.yfolable.append(ytable)
        self._tfpnaprf = tfpnaprf(self.predfolable, self.yfolable, 'oc', 'rc')
        self._clfboard = clfboard
        self._clfscoreboard = clfscoreboard
        pass

    def onOARTesting(self) : 
        cn = 10e+10
        parfe = []
        totalvaliddatalen = reduce(lambda x, y : x + y , [len(cs._vadata) for cs in self._cslist])
        tfpnlist = []
        tfpntable = []
        emr = 0
        predtable, ytable = [], []
        for i, cs in enumerate(self._cslist) : 
            ylist = []
            pred = []
            resKernelList = self.getRestKernels(cs._name)
            oneKernelList = cs._kslist
            tfpnlist = []
            for vcs in self._cslist : 
                y = ['oc' if vcs._name in cs._name else 'rc' for x in vcs._vadata]
                for vd in vcs._vadata : 
                    opplist = [kn.postprob(vd) for kn in oneKernelList]
                    rpplist = [kn.postprob(vd) for kn in resKernelList]
                    
                    '''
                    Let compare Beta ppf of one and rest then find largest ppf.
                    Largest beta ppf indicates of which the class predict. 
                    '''
                    opp, rpp = max(opplist), max(rpplist)
                    
                    if opp >= rpp : 
                        pn = True
                        tf = True if cs._name is vcs._name else False
                        if tf : 
                            emr += 1
                        pred.append('oc')
                    else : 
                        pn = False
                        tf = False if cs._name is vcs._name else True
                        pred.append('rc')
                    tfpnlist.append([tf, pn])
                    pass
                ylist.extend(y)
                pass
            ytable.append(ylist)
            predtable.append(pred)
            tfpntable.append(tfpnlist)
            
            pass
        parfe = tfpnlist_parf(tfpntable, avetype='micro')
        parfe.append(float(emr) / float(len(self._cslist) * len(cs._vadata)))

        return parfe, predtable, ytable

    def onTesting(self, fold) : 
        '''
        test on each fold
        return ditance on each norm for each kernel
        '''
        dmlist = []
        for i, tcs in enumerate(self._cslist) : 
            dmap = []
            for vcs in self._cslist : 
                kn = vcs._kslist
                dlist = tcs.onTesting(kn)
                dmap.append(dlist)
            dmlist.append(dmap)
            fn = '#%d%s' % (fold + 1, tcs._name)
            
        return dmlist

    def getRestKernels(self, onename) : 
        rkn = []
        for vd in self._cslist : 
            if onename is not vd._name : 
                rkn.extend(vd._kslist)
        return rkn

    def learnSVM(self) : 
        print('learning SVM')
        self._parfelist = []
        self.predfolable = []
        self.yfolable = []
        
        ffdata = [[] for f in range(self._fmax)]
        ffname = [[[] for cs in self._cslist] for f in range(self._fmax)]
        for fold in range(self._fmax) :
            print('fold#%d' % fold)
            
            restrdata = []
            for cs in self._cslist : 
                cs.onFolding(fold)
            ##set training data of rest class
            fittingdata= []
            for i, cs in enumerate(self._cslist) : 
                ffdata[fold].extend(cs._trdata)
                for j, cscs in enumerate(self._cslist) : 
                    ffname[fold][i].extend(['oc' if cs._name is cscs._name else 'rc' for x in cscs._trdata])
            for i, cs in enumerate(self._cslist) : 
                size = 100
                cs.initSklearnParam(ffdata[fold], ffname[fold][i])
                pass
            pred, predtable, ytable = self.onSVMTesting(fold)
            self.predfolable.append(predtable)
            self.yfolable.append(ytable)
            self._parfelist.append(pred)
        self._tfpnaprf = tfpnaprf(self.predfolable, self.yfolable, 'oc', 'rc')

    def onSVMTesting(self, fold) : 
        plist = []
        tfpnlist, emrlist, errs = [], [], []
        clf = SVC(kernel='rbf', gamma=50)
        predtable = []
        ytable = []
        for tcs in self._cslist : 
            #print(time.strftime('%X', time.localtime()) + (' fold%02d, ' % fold) + tcs._name + '=>svm testing'
            data = [[y for y in x] for x in tcs._fitdata]
            name = [x for x in tcs._fitname]
            clf.fit(data, name)
            pred = clf.predict(data)
            err = metrics.accuracy_score(name, pred)
        
            #print('err=%lf'%err
            vatarget, valist = [], []
            for vcs in self._cslist : 
                valist.extend([x for x in vcs._vadata])
                vatarget.extend(['oc' if tcs._name is vcs._name else 'rc' for x in vcs._vadata])
            pred = clf.predict(valist)
            predtable.append(pred)
            ytable.append(vatarget)
            #acc = metrics.accuracy_score(vatarget, pred)
            #pre = metrics.precision_score(vatarget, pred, pos_label='oc')
            #rec = metrics.recall_score(vatarget, pred, pos_label='oc')
            #f1m = metrics.f1_score(vatarget, pred, pos_label='oc')
            #clfr = metrics.classification_report(vatarget, pred)
            tfpn, emr = skl_tfpn(pred, vatarget, 'oc', 'rc', isEMR = True)
            tfpnlist.append(tfpn)
            emrlist.append(emr)
            errs.append(err)
            #plist.append([acc, pre, rec, f1m])
            pass
        ##print('emr=%d'%sum(emrs)
        errs, aveemr = np.mean(errs), np.mean(emrlist)/len(tcs._vadata)
        #print('err=%lf'% errs
        parfe = []
        parfe = tfpnlist_parf(tfpnlist, avetype = 'macro')
        parfe.append(aveemr)
        return parfe, predtable, ytable
        #return plist

    def learnKNN(self) : 
        print('learning Nearest Neighbor')
        aprnlist = []
        self._parfelist = []
        self.predfolable = []
        self.yfolable = []

        ffdata = [[] for f in range(self._fmax)]
        ffname = [[[] for cs in self._cslist] for f in range(self._fmax)]
        for fold in range(self._fmax) :
            print('fold#%d' % fold)
            
            restrdata = []
            for cs in self._cslist : 
                cs.onFolding(fold)
            ##set training data of rest class
            fittingdata= []
            for i, cs in enumerate(self._cslist) : 
                ffdata[fold].extend(cs._trdata)
                for j, cscs in enumerate(self._cslist) : 
                    ffname[fold][i].extend(['oc' if cs._name is cscs._name else 'rc' for x in cscs._trdata])
            for i, cs in enumerate(self._cslist) : 
                size = 100
                cs.initSklearnParam(ffdata[fold], ffname[fold][i])
                pass
            pred, predtable, ytable = self.onKNNTesting(fold)
            self.predfolable.append(predtable)
            self.yfolable.append(ytable)
            self._parfelist.append(pred)
        self._tfpnaprf = tfpnaprf(self.predfolable, self.yfolable, 'oc', 'rc')
        pass

    def onKNNTesting(self, fold) : 
        plist = []
        tfpnlist, emrlist = [], []
        predtable, ytable = [], []
        
        clf = KNeighborsClassifier(weights='distance')
        knnscoreboard = []
        for tcs in self._cslist : 
            #print(time.strftime('%X', time.localtime()) + (' fold%02d, ' % fold) + tcs._name + '=>knn testing'
            data = [[y for y in x] for x in tcs._fitdata]
            name = [x for x in tcs._fitname]
            clf.fit(data, name)

            vatarget, valist = [], []
            for vcs in self._cslist : 
                valist.extend([x for x in vcs._vadata])
                vatarget.extend(['oc' if tcs._name is vcs._name else 'rc' for x in vcs._vadata])
            pred = clf.predict(valist)
            predtable.append(pred)
            ytable.append(vatarget)
            #acc = metrics.accuracy_score(vatarget, pred)
            #pre = metrics.precision_score(vatarget, pred, pos_label='oc')
            #rec = metrics.recall_score(vatarget, pred, pos_label='oc')
            #f1m = metrics.f1_score(vatarget, pred, pos_label='oc')
            #clfr = metrics.classification_report(vatarget, pred)
            tfpn, emr = skl_tfpn(pred, vatarget, 'oc', 'rc', isEMR = True)
            tfpnlist.append(tfpn)
            emrlist.append(emr)
            #plist.append([acc, pre, rec, f1m])
            pass
            #print(aprf
            knnscoreboard.append(pred)
            pass
        aveemr = np.mean(emrlist)/len(tcs._vadata)
        
        parfe = tfpnlist_parf(tfpnlist, avetype = 'micro')
        parfe.append(aveemr)
        return parfe, predtable, ytable
        #return plist
    pass


class cspace : 
    '''
    radar category
    '''
    def __init__(self, name, features, foldmax, knvol = 1) : 
        self._name = name
        self._FeatureList = features
        self._ftvolume = len(features)
        self._vavolume = int(float(self._ftvolume) / float(foldmax))
        self._trvolume = self._ftvolume - self._vavolume
        self._KernelSize = knvol
        self._fitdata = []
        self._fitname = []
        pass

    def onFolding(self, cnt) : 
        '''
        method on folding
        set training data and valid data
        '''
        self._Fold = cnt
        self._kslist = []
        aindex = cnt * self._vavolume
        bindex = aindex + self._vavolume
        trdata = self._FeatureList[:aindex] + self._FeatureList[bindex:]
        vadata = self._FeatureList[aindex : bindex]
        
        trtr = list(zip(*trdata))
        mean, var, dev = [], [], []
        for trtrrow in trtr : 
            m = reduce(lambda x, y: x + y, trtrrow) / len(trtrrow)
            mean.append(m)
            v = 0
            for row in trtrrow : 
                v += (row - m) ** 2
                v /= self._trvolume - 1
            var.append(v)
            dev.append(v ** 0.5)

        self._mean = mean
        self._var = var
        self._dev = dev
        self._trdata = trdata
        self._vadata = vadata
        self._fitdata = None
        #self.ctrddrt()
        pass

    def onTraining(self) : 
        #positioning kernel position for kernel size 1
        #if you want to set more kernel, you take method 'dalken(data allocate on kernel)'
        kpos = self._mean
        ks = kspace(self._mean, self._trdata, self._name)

        self._kslist.append(ks)
        pass

    def onTesting(self, tck) : 
        '''
        test on target class kernel
        '''
        dmap = []
        for vd in self._vadata : 
            dlist = []
            for kernel in tck : 
                d = kernel.postprob(vd)
                dlist.append(d)
            dmap.append(dlist)
            pass
        return dmap

    def initSklearnParam(self, data, name) : 
        self._fitdata = data
        self._fitname = name
        pass

    def ctrddrt(self, bins = 2, peaks = 2) :
        """
        set centroid direction 
        peaks : int.
        number of modals. if you need bimodal which means you need two kernels, then peaks are 2
        """
        dimdata = list(zip(*self._trdata))
        #1.get min and max of every dimension
        minlist = [min(x) for x in dimdata]
        maxlist = [max(x) for x in dimdata]
        dmlist = self._mean
        dvlist = self._var

        dimpeaks = []
        for dimlist, dm, dv in list(zip(dimdata, dmlist, dvlist)) : 
            inc, dec = False, False
            bound = False
            y = [x for x in dimlist]
            leny = float(len(y))
            y.sort()
            miny, maxy = y[0], y[-1]
            buffer, lenbuffer, pv = [], 0., 0.
            peaks = []
            for dim in y : 
                buffer.append(dim)
                lenbuffer += 1.
                if lenbuffer > 1 : 
                    m = sum(buffer) / lenbuffer
                    v = sum([0 if x == m else (x - m)**2 for x in buffer]) / lenbuffer
                    if pv > v : 
                        dec = True
                    elif dec and pv < v : 
                        peaks.append(buffer[:-1])
                        buffer, m, v, dec, lenbuffer = [buffer[-1]], 0, 0, False, 0
                    pv = v
            peaks.append(buffer)
            dimpeaks.append(peaks)
            pass
        lenpeaks = reduce(lambda x, y : x * y, [len(k) for k in dimpeaks])
                
            
        #chunking data on same variance
        chunks = []
        

        #get Probability Cumulative Distribution Function
        
        
        #get priority table sorted by variance
        varlist = [[i, x] for i, x in enumerate(self._var)]
        varlist.sort(key=lambda x : x[1])#sort by var in order to increase

        # 
        pass
    pass

class kspace : 
    '''
    Kernel on centroid
    '''
    def __init__(self, ctrd_pos, trd, name='noname') : 
        self._pos = ctrd_pos
        ###get statistics : mean and variance
        self._name = name
        self._data = np.array(trd)
        datatr = np.array(list(zip(*trd)))
        self._mean, self._var = [m.mean() for m in datatr], [v.var(ddof=1) for v in datatr]
        ###get statistics ends
        self.dimlen = len(trd[0])
        self.rangedimlen = range(self.dimlen)
        self._p3c = self.discriminant()
        
        #IGNORE
        #betaCDF = beta.cdf(y, y_beta_a, y_beta_b)
        #betaPDF = beta.pdf(y, y_beta_a, y_beta_b)
                
        #lkep.plotPDF(y, rCat._name)
        #lkep.plotCDF(y, rCat._name)
        #lkep.plotKStest(y, ecdf, betaCDF, y_beta_a, y_beta_b, y_pval, rCat._name)
        #lkep.plotBetaPDF(trNorm_fs_Beta_a, trNorm_fs_Beta_b, rCat._name)
        #lkep.plotBetaCDF(trNorm_fs_Beta_a, trNorm_fs_Beta_b, rCat._name)
        pass
    
    def discriminant(self) : 
        """
        discriminant function 
        paramters
        ---------
        3435
        returns
        -------
        p3c : p3c object
        It has p-value, D, Y, alpha and beta of beta parameter, eCDF, beta CDF
        """
        trlen = len(self._data)
        lenswc = 5
        swczero = int(float(lenswc) / 2.)
        rangeswc = range(lenswc)
        rangedimlen = range(self.dimlen)
        swc_map = [[2**(x - swczero) for y in rangedimlen] for x in rangeswc]
        ecdf = [float(x) / float(trlen) for x in range(1, trlen + 1)]
        nmnt = [0.]
        mct = 0

        #step 1. find MSD(Most Significant Dimension)
        len_nomi = 5
        len_dim = self.dimlen
        rangedimlen = self.rangedimlen
        
        for swc in swc_map : 
        #while True : 
            mct += 1
            bcdfparams = self.getbcdf_pval_swc(swc)
            if not isinstance(bcdfparams, p3c) : 
                continue

            if nmnt[0] < bcdfparams._pval:# and bcdfparams._pval >= 0.05 : #0.9 is similiar to 0.99
            #if True : 
                #print('Monte-carlo try : %d' % mct
                nmnt = [bcdfparams._pval, bcdfparams._d, bcdfparams._Y, swc, bcdfparams._betaA, bcdfparams._betaB, ecdf, bcdfparams._betaCDF]
                #nmnt = [pval, d, Y, swc, bafit, bbfit, ecdf, betacdf]
                #break#Monte-Carlo
            pass
        
        #lkep.plotKStest(nmnt[0], nmnt[2], nmnt[4], nmnt[5], nmnt[6], nmnt[7], 'mc_'+self._name)
        return p3c(nmnt[0], nmnt[1], nmnt[2], nmnt[3], nmnt[4], nmnt[5], nmnt[6], nmnt[7])


    def postprob(self, valid) : 
        y = self.kernelizeisw(valid, self._p3c._sw)
        pval = self._p3c.mapy2beta(y)
        return pval

    def kernelizeisw(self, X, sw) :
        """
        kernelize with indipentent sigma weight
        """
        h_x = 0.
        dsquare = 0.
        xmvw = list(zip(X, self._mean, self._var, sw))
        dsquare = 0.5 * reduce(lambda p, q : p + q, [((x - m) ** 2) / ((w ** 2) * v) for x, m, v, w in xmvw])
                
        h_x = math.exp(-1 * dsquare)
        
        return h_x

    def getbcdf_pval_swc(self, swc) : 
        """
        get p-value of beta CDF with candidated sigma weight
        parameters
        ----------

        returns
        -------
        pval : 
        d : 
        Y : 
        ba : 
        bb : 
        bcdf : beta.cdf
        """
        Y = [self.kernelizeisw(x, swc) for x in self._data]

        Y.sort()
        y_m = np.mean(Y)
        y_v = np.var(Y, ddof = 1)
        if math.isnan(y_v) or y_v == 0 : 
            return 0

        ba = y_m ** 2 * ((1 - y_m) / y_v - 1 / y_m)
        bb = ba * (1 - y_m) / y_m
        bcdf = beta.cdf(Y, ba, bb)
                    
        #Y = featureScaling(Y)
        d, pval = scistats.kstest(Y, lambda cdf : bcdf)
        
        params = p3c(pval, d, Y, [x for x in swc], ba, bb, 0, bcdf)
        return params

    pass
    
class p3c : 
    """
    Posterior Probability Parameters of Class
    """
    def __init__(self, pval, d, Y, sw, ba, bb, ecdf, bcdf):
        self._pval = pval
        self._d = d
        self._Y = Y
        self._ymin = min(Y)
        self._ymax = max(Y)
        self._sw = sw#sigma weight
        self._betaA = ba
        self._betaB = bb
        self._ecdf = ecdf
        self._betaCDF = bcdf

    def mapy2beta(self, y) : 
        """
        mapping y to beta
        
        parameters
        ----------
        self : class object
            hypothesis criteria

        y : array_like
            Input array

        returns
        -------
        pair of beta CDF : Float 
            Returns a float.
        """
        a = beta.cdf(y, self._betaA, self._betaB)
        i = 0
        if y < self._ymin : 
            return 0
        elif y > self._ymax : 
            return 1
        else : 
            for i, z in enumerate(self._Y) : 
                if y < z : 
                    i = self._Y.index(z)
                    break
        return a
        #return self._betaCDF[i]
    pass

def getNorm(row, mean, var) : 
    """
    Euclidean norm calculator

    """
    d = reduce(lambda x, y : x + y , [(i - m) ** 2 / v for i, m, v in list(zip(row, mean, var))])
    return d ** 0.5

def scoreclf(distancemap) : 
    sb, map = [], [list(zip(*tcmap)) for tcmap in distancemap]
    for eclist in map : 
        ecsb = []
        for klist in eclist : 
            v = max([max(a) for a in klist])
            clsidx = next((i for i, sublist in enumerate(klist) if v in sublist), -1)
            ecsb.append(clsidx)
        sb.append(ecsb)
        pass
    return sb

def boardclf(clfscrlist) : 
    board = [[0 for y in clfscrlist] for x in clfscrlist]
    for i, tgcs in enumerate(clfscrlist) : 
        for cs in tgcs : 
            board[i][cs] += 1
            pass
    return board

def getPairwiseDistances(arrx, arry, arrvar) : 
    '''
    Pairwise Distance
    each array MUST have same length.
    this method uses standard deviation on feature scaling
    '''
    devMax ,devMin = max(arrvar), min(arrvar)
    devran = devMax - devMin
    fsvar = [1 - (dev - devMin) / devran for dev in arrvar]#arrvar

    d = 0.

    param = []
    for i in range(len(fsvar)) : 
        param.append([arrx[i], arry[i], arrvar[i], fsvar[i]])
    
    for (x, y, dev, fs) in param :
        d += dev != 0 and fs * ((x - y) ** 2) / dev or 0

    return d ** 0.5

def mapctrd(cslist, fold) : 
    '''
    calculate and list classes of centroids
    works in the fold
    '''
    map = [[0. for y in cslist] for x in cslist]
    for i, acs in enumerate(cslist) : 
        for j, bcs in enumerate(cslist) : 
            map[i][j] = getNorm(bcs._kslist[0]._pos, acs._mean, acs._var)
 
    return map

def getAPRF(tfpnlist) : 
    '''get accuracy, precision, recall, F_1 measure
    listing by data and data composed on T/F, P/N
    '''
    tp, fp, tn, fn = 0, 0, 0, 0
    for tf, pn in tfpnlist : 
        if tf and pn: 
            tp += 1
        elif not tf and pn : 
            fp += 1
        elif tf and not pn : 
            tn += 1
        elif not tf and not pn : 
            fn += 1
    tp, fp, tn, fn = float(tp), float(fp), float(tn), float(fn)
    acc = (tp+tn)/(tp+fp+tn+fn)
    pre = tp/(tp+fp)
    rec = tp/(tp+fn)
    fme = 0.0 if pre + rec == 0 else (2 * pre * rec) / (pre + rec)

    return acc, pre, rec, fme

def featureScaling(arr) : 
    """
    feature scaling
    parameters
    ----------
    arr : numeric array-like

    returns
    -------
    arr : numeric array-like
    feature scaled array
    """
    xmin, xmax = min(arr), max(arr)
    if xmax != 0 and xmax - xmin != 0 : 
        arr = [(x - xmin) / (xmax - xmin) for x in arr]
    return arr


def pcdf(a, bins = 10) : 
    """
    Probability Cumulative Distribution Function
    parameters
    ----------
    list : array-like
    bins : int or sequence of scalars, optional
    If bins is an int, it defines the number of equal-width bins 
    in the given range (10, by default). If bins is a sequence, 
    it defines the bin edges, including the rightmost edge, 
    allowing for non-uniform bin widths.

    returns
    -------
    CDF : array-like
    """
    cdfa = range(len(a))
    lena = len(a)
    y = [x for x in a]
    y.sort()
    binlist = []

    if isinstance(bins, int) : 
        
        return binlist
    elif isinstance(bins, list) : 
        pass

def chunk(a, bins) : 
    lena = len(a)
    ratio = float(lena) / float(bins)
    can = []
    
    for i in range(bins) : 
        start = int(math.ceil(i * ratio))
        end = int(math.ceil((i + 1) * ratio))
        bin = a[start : end]
        can.append(bin)

    return can

def skl_tfpn(pred, target, posname, negname, isEMR = False) : 
    emrlist, tfpn = [], []
    tf, pn = False, False
    emr = 0
    for p, t in list(zip(pred, target)) : 
        if p in posname : 
            pn = True
            if t in p : 
                tf = True
                emr += 1
            else : 
                tf = False

        elif p in negname : 
            pn = False
            tf = True if t in p else False
        else : 
            'wtf?'
        tfpn.append([tf, pn])

    return [tfpn, emr] if isEMR else tfpn

def tfpnaprf(predfolable, yfolable, pname, fname) : 
    """
    predfolable : predict fold table
    yfolable : y fold table
    """
    tfpnfolable, macrofolable, microfolable = [], [], []

    for predtable, ytable in list(zip(predfolable, yfolable)) : 
        tfpntable, macrotable = [], []
        for pred, y in list(zip(predtable, ytable)) : 
            tp, tn, fp, fn = 0, 0, 0, 0
            for p, t in list(zip(pred, y)) : 
                if p in pname : 
                    if t in p : 
                        tp += 1
                    else :
                        fp += 1
                elif p in fname : 
                    if t in p : 
                        tn += 1
                    else : 
                        fn += 1
                else : 
                    'wtf?'
            ttff = [tp, tn, fp, fn]
            tfpntable.append(ttff)

            a, p, r = accuracy(tp, tn, fp, fn), precision(tp, fp), recall(tp, fn)
            f = f1measure(p, r)
            macrotable.append([a, p, r, f])
            pass
        tfpnfolable.append(tfpntable)
        macrofolable.append(macrotable)

    ztfpn  = [[    sum(bcr) for bcr in list(zip(*cls))] for cls in list(zip(*tfpnfolable))]
    zmacro = [[np.mean(bcr) for bcr in list(zip(*cls))] for cls in list(zip(*macrofolable))]
    for tp, tn, fp, fn in ztfpn : 
        a, p, r = accuracy(tp, tn, fp, fn), precision(tp, fp), recall(tp, fn)
        f = f1measure(p, r)
        microfolable.append([a, p, r, f])
    table = [[tfpn[0], tfpn[1], tfpn[2], tfpn[3], micro[0], micro[1], micro[2], micro[3], macro[0], macro[1], macro[2], macro[3]] for tfpn, micro, macro in zip(ztfpn, microfolable, zmacro)]

    return table

def accuracy(tp, tn, fp, fn) : 
    return 0 if tp+tn+fp+fn == 0 else float(tp+tn)/float(tp+tn+fp+fn)

def precision(tp, fp) : 
    return 0 if tp+fp == 0 else float(tp) / float(tp + fp)

def recall(tp, fn) : 
    return 0 if tp+fn == 0 else float(tp) / float(tp + fn)

def f1measure(p, r) : 
    return 0 if p+r == 0 else (2 * p * r)/(p + r)

def tfpnlist_parf(tplist, avetype = 'macro') : 
    parf = []
    tptnfpfn = []
    for tfpn in tplist : 
        tp, tn, fp, fn = 0, 0, 0, 0
        for tf, pn in tfpn : 
            if tf and pn : 
                tp += 1
            elif tf and not pn : 
                tn += 1
            elif not tf and pn : 
                fp += 1
            elif not tf and not pn :
                fn += 1
            else : 
                "wtf?"
        tptnfpfn.append([tp, tn, fp, fn])
        pass
    lentptnfpfn = float(len(tptnfpfn))
    a = sum(float(tp + tn)/float(tp + tn + fp + fn) for tp, tn, fp, fn in tptnfpfn)/float(lentptnfpfn)
    if avetype is 'macro' : 
        p, r, f = 0., 0., 0.
        for tp, tn, fp, fn in tptnfpfn : 
            p += 0.0 if tp+fp == 0 else float(tp) / float(tp+fp)
            r += 0.0 if tp+fn == 0 else float(tp) / float(tp+fn)
        p /= lentptnfpfn
        r /= lentptnfpfn
        pass
    elif avetype is 'micro' : 
        tp, tn, fp, fn = [sum(x) for x in list(zip(*tptnfpfn))]
        p = 0.0 if tp+fp == 0 else float(tp) / float(tp+fp)
        r = 0.0 if tp+fn == 0 else float(tp) / float(tp+fn)
        pass
    else : 
        print('input avetype as micro or macro')
        return 0
    f = 0.0 if p + r == 0.0 else (2*p*r)/(p+r)
    parf = [p, a, r, f]
    
    return parf
