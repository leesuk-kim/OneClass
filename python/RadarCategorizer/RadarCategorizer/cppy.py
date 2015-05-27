'''
Radar signal Categorizer
version 1.0
Designed by leesuk kim(aka. LK)
'''
import time
import numpy as np
import numpy.linalg as npla
from scipy.stats import beta
import scipy.stats as scistats
import lkpy as lkep
import math
from sklearn.svm import SVC
from sklearn import metrics
from sklearn.neighbors import NearestNeighbors
import gc

CATArr_INDEX_NAME = 0
CATArr_INDEX_TRAIN = 1
CATArr_INDEX_TEST = 2
CATArr_INDEX_CAT = 1

class cpon : 
    '''
    Class Probability Output Network
    '''
    
    def __init__(self, fold = 2, srcdir='') : 
        self._TimeStamp = '%d' % int(time.time())
        self._CategoryArr = []
        '''raw matrix'''
        self._ClsVol = 0
        self._CatDim = 1
        self._cslist = []
        self._fmax = fold
        self._kmax = 1
        self._cslist = []
        self._ctrdmap = []
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

    def appendTrainRow(self, row) : 
        '''
        append Raw row
        '''
        name = row[0]
        seq = row[1:]

        cat = next((rrow for i, rrow in enumerate(self._CategoryArr) if name in rrow), False)
        if not cat : 
            print 'create new category : %s' % name
            self._CategoryArr.append([name, [seq]])
            self._ClsVol += 1
        else : 
            cat[1].append(seq)

    def appendTrain(self, matrix) : 
        try : 
            self.appendTrainRow(matrix[x] for x in range(len(matrix)))
        except TypeError : 
            print 'please input a vector.'

        pass

    def appendValidRow(self, row) : 
        #append test data
        pass
    def appendValid(self, matrix) : 
        try : 
            self.appendValidRow(matrix[x] for x in range(len(matrix)))
        except TypeError : 
            print 'please input a vector.'

        pass

    def setFold(self, fold) : 
        self._fmax = fold

    def getcsnames(self) : 
        return [cs._Name for cs in self._cslist]
        
    def getKnVol(self):
        return self._kmax
    def setKnVol(self, knvol) : 
        '''if it has fixed number of kernel, then put number in.
        if grow is true, it makes kernel volmes most effective.
        '''
        self._kmax = knvol

    def getcslist(self, idx) : 
        return self._cslist[idx]

    def learn(self) : 
        print 'learning'
        fsblist, fstatslist = [], []
        self._clfAPRF = []
        for fold in range(self._fmax) : 
            print 'fold#%d' % fold

            for cs in self._cslist : 
                cs.onFolding(fold)
                cs.onTraining()

            fctrdmap = mapctrd(self._cslist, self._fmax)
            self._ctrdmap.append(fctrdmap)

            foldsb = boardclf(scoreclf(self.onTesting(fold)))
            fsblist.append(foldsb)
            self._clfAPRF.append(self.onOARTesting())
        self._clfboard = fsblist
        self._clfstatslist = fstatslist
        pass

    def onOARTesting(self) : 
        cn = 10e+10
        stats = []
        totalvaliddatalen = reduce(lambda x, y : x + y , [len(cs._vadata) for cs in self._cslist])
        tfpnlist = []

        for i, cs in enumerate(self._cslist) : 
            resKernelList = self.getRestKernels(cs._Name)
            oneKernelList = cs._kslist

            for vcs in self._cslist : 
                for vd in vcs._vadata : 
                    oclostlist = [getNorm(vd, kn._pos, kn._var) for kn in oneKernelList]
                    rclostlist = [getNorm(vd, kn._pos, kn._var) for kn in resKernelList]
                    oclost, rclost = min(oclostlist), min(rclostlist)

                    if cs._Name is vcs._Name : 
                        pn = True
                        tf = True if oclost < rclost else False
                    else : 
                        pn = False
                        tf = True if oclost > rclost else False

                    tfpnlist.append([tf, pn])
                    pass
                pass
            pass
        stats = getAPRF(tfpnlist)
        return stats

    def onTesting(self, fold) : 
        '''
        test on each fold
        return ditance on each norm for each kernel
        '''
        dmlist = []
        for i, tcs in enumerate(self._cslist) : 
            dmap = []
            for vcs in self._cslist : 
                kn = vcs.getKernels()
                dlist = tcs.onTesting(kn)
                dmap.append(dlist)
            dmlist.append(dmap)
            fn = '#%d%s' % (fold + 1, tcs.getName())
            
        return dmlist

    def getRestKernels(self, onename) : 
        rkn = []
        for vd in self._cslist : 
            if onename is not vd._Name : 
                rkn.extend(vd._kslist)
        return rkn

    def learnSVM(self) : 
        print 'learning SVM'

        originsvm = SVC(kernel='linear')
        
        for fold in range(self._fmax) :
            print 'fold#%d' % fold
            
            restrdata = []
            for cs in self._cslist : 
                cs.onFolding(fold)

            ##set training data of rest class
            for i, cs in enumerate(self._cslist) : 
                tdlist = []
                for vcs in self._cslist : 
                    if vcs._Name is not cs._Name : 
                        tdlist.extend(vcs._trdata)
                fittingdata = [x for x in cs._trdata]
                fittingdata.extend(tdlist)
                fittingname = ['one' for x in cs._trdata]
                fittingname.extend(['rest' for x in tdlist])
                print time.strftime('%X', time.localtime()) + (' fold%02d, ' % fold) + cs._Name + '=>initSVM'
                #cs.initSVM([[i + x + y for y in range(1, 25)] for x in range(10)], ['one' if w % 2 == 0 else 'rest' for w in range(10)])
                cs.initSVM(fittingdata, fittingname)
                pass
            pred = self.onSVMTesting(fold)
        return fsblist

    def onSVMTesting(self, fold) : 
        plist = []

        for tcs in self._cslist : 
            print time.strftime('%X', time.localtime()) + (' fold%02d, ' % fold) + tcs._Name + '=>svm testing'
            vatarget = []
            valist = []
            for vcs in self._cslist : 
                valist.extend([x for x in vcs._vadata])
                vatarget.extend(['one' if tcs._Name is vcs._Name else 'rest' for x in vcs._vadata])
            pred = tcs._svm.predict(valist)
            acc = metrics.accuracy_score(vatarget, pred)
            pre = metrics.precision_score(vatarget, pred, pos_label='one')
            rec = metrics.recall_score(vatarget, pred, pos_label='one')
            f1m = metrics.f1_score(vatarget, pred, pos_label='one')
            plist.append([acc, pre, rec, f1m])
            pass

        return plist

    def learnKNN(self) : 
        print 'learning Nearest Neighbor'

        fsblist = []
        cslen = len(self._cslist)
        for fold in range(self._fmax) : 
            print 'fold#%d' % fold

            fitdatalist = []
            fitnamelist = []
            for cs in self._cslist : 
                cs.onFolding(fold)
                fdlist = cs.getTrainData()
                csn = cs.getName()
                fnlist = [csn for td in fdlist]
                fitdatalist.extend(fdlist)
                fitnamelist.extend(fnlist)
            #training
            self._knn = NearestNeighbors(n_neighbors=cslens).fit(fitdatalist)

            #distances, indices = knn.kneighbors(fitdatalist)
            foldsb = self.onKNNTesting()#testing
            fsblist.append(foldsb)
        return fsblist

    def onKNNTesting(self) : 
        plist = []
        
        for i, tcs in enumerate(self._cslist) : 
            for vd in tcs.getValidData() : 
                dist, indice = self._knn.kneighbors(vd)
                #plist.append([tcs.getName(), pred[0]])
            pass
        #self._knn.kneighbors
        pass
    pass


class cspace : 
    '''
    radar category
    '''
    def __init__(self, name, features, foldmax, knvol = 1) : 
        self._Name = name
        self._FeatureList = features
        self._ftvolume = len(features)
        self._vavolume = int(float(self._ftvolume) / float(foldmax))
        self._trvolume = self._ftvolume - self._vavolume
        self._KernelSize = knvol
        pass

    def getName(self):
        return self._Name

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
        
        trtr = zip(*trdata)
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
        pass

    def onTraining(self) : 
        #positioning kernel position for kernel size 1
        #if you want to set more kernel, you take method 'dalken(data allocate on kernel)'
        kpos = self._mean
        self._kslist.append(kspace(kpos, self._trdata, self._Name))
        pass

    def onTesting(self, tck) : 
        '''
        test on target class kernel
        '''
        dmap = []
        for vd in self._vadata : 
            dlist = []
            for kernel in tck : 
                d = getNorm(vd, kernel.getMean(), kernel.getVar())
                dlist.append(d)
            dmap.append(dlist)
            pass
        return dmap

    def setKernel(self) : 
        '''
        set centroid
        get Centroid position and set that pos. on each kernels
        in Radar Categorizer, we take ONLY ONE centroid.
        '''
        self.initCtrdPos()

        self._CentroidArr = []

        for knl in range(self._KernelSize) : 
            self._CentroidArr.append(cpon.cspace.kspace(self._trMean, self))
        pass

    def initCtrdPos(self) : 
        pos = []
        pass

    def getMean(self) : 
        return self._mean
    def getVar(self) : 
        return self._var
    def getDev(self) : 
        return self._dev

    def getTrainData(self) : 
        return self._trdata

    def getValidData(self) : 
        return self._vadata

    def getKernels(self) : 
        return self._kslist

    def initSVM(self, data, name) : 
        self._svm = SVC(kernel='linear')
        self._svm.fit(data, name)
        pass
    pass

class kspace : 
    def __init__(self, ctrd_pos, trd, name='noname') : 
        self._pos = ctrd_pos
        #self._Category = scs
        ###get statistics : mean and variance
        self._name = name
        self._data = np.array(trd)
        datatr = np.array(zip(*trd))
        mean, var = [], []
        for dtr in datatr : 
            m = dtr.mean()
            v = dtr.var(ddof = 1)
            mean.append(m)
            var.append(v)
         ###get statistics ends
        normlist = [getNorm(row, mean, var) for row in trd]
        self._mean = mean
        self._var = var
        self._normlist = normlist
        #self._trNorm = trNorm = [npla.norm(ctrd_pos - x) for x in self._data]
        #NOT YET...
        #self._lentr = lentr = len(trNorm)
        #self._trNorm_mean = trNorm_mean = np.mean(trNorm)
        #self._trNorm_var = trNorm_var = np.var(trNorm, ddof = 1)
        #featureScaling = trNorm
        #fsmax, fsmin = max(featureScaling), min(featureScaling)
        #featureScaling.sort()
        #self._trNorm_fs = trNorm_fs = [(x - fsmin) / (fsmax - fsmin) for x  in featureScaling]
                
        #y_pval, y_d, y_sigma, y_beta_a, y_beta_b , ecdf, betacdf = self.getKernel()
        #y = self.setKernel(y_sigma)
        #ecdf = [float(x) / float(lentr) for x in range(1, lentr + 1)]
        #betaCDF = beta.cdf(y, y_beta_a, y_beta_b)
        #betaPDF = beta.pdf(y, y_beta_a, y_beta_b)
                
        #lkep.plotPDF(y, rCat._Name)
        #lkep.plotCDF(y, rCat._Name)
        #lkep.plotKStest(y, ecdf, betaCDF, y_beta_a, y_beta_b, y_pval, rCat._Name)
        #lkep.plotBetaPDF(trNorm_fs_Beta_a, trNorm_fs_Beta_b, rCat._Name)
        #lkep.plotBetaCDF(trNorm_fs_Beta_a, trNorm_fs_Beta_b, rCat._Name)
        pass
    def getMean(self) : 
        return self._mean
    def getVar(self) : 
        return self._var
    def getName(self) :
        return self._name

    def setKernel(self, sigma) :
        y = [math.exp(-1 * (x ** 2) / (2 * sigma ** 2)) for x in self._trNorm]
        return [1 - yi for yi in y]

    def getKernel(self) : 
        sigma_cddt = [2 ** (i - 4) for i in range(7)]
        nmnt = [0., 0., 0., 0., 0., 0., 0.]
        trlen = len(self._trNorm)

        for sigma in sigma_cddt : 
            y = self.setKernel(sigma)
            y_m = np.mean(y)
            y_v = np.var(y, ddof = 1)
            ba = y_m ** 2 * ((1 - y_m) / y_v - 1 / y_m)
            bb = ba * (1 - y_m) / y_m
            bafit, bbfit, loc, scal = beta.fit(y, ba, bb)
            for x in range(20) : 
                bafit, bbfit, loc, scal = beta.fit(y, bafit, bbfit)
            betacdf = beta.cdf(y, bafit, bbfit)
                    
            ymax, ymin = max(y), min(y)
            ecdf = [float(x) / float(trlen) for x in range(1, trlen + 1)]
            #WHAT IS 'CALLABLE'???
            #d, pval = scistats.ks_2samp(ecdf, betacdf)
            d, pval = scistats.kstest(ecdf, lambda cdf : betacdf)

            if nmnt[0] < pval : 
                nmnt = [pval, d, sigma, bafit, bbfit, ecdf, betacdf]
        lkep.plotKStest(y, nmnt[5], nmnt[6], nmnt[3], nmnt[4], nmnt[0], self._Category._Name)

        return nmnt

    def getCentroid(self) : 
        return self._pos
    pass

def getNorm(row, mean, var) : 
    d = 0.
    d = reduce(lambda x, y : x + y , [(i - m) ** 2 / v for i, m, v in zip(row, mean, var)])
    #for i, m, v in zip(row, mean, var) : 
    #    d += (i - m) ** 2 / v
    return d ** 0.5

def scoreclf(distancemap) : 
    sb, map = [], [zip(*tcmap) for tcmap in distancemap]
    for eclist in map : 
        ecsb = []
        for klist in eclist : 
            v = reduce(lambda x, y : x if x < y else y, min(klist))
            i = klist.index(v)
            ecsb.append(i)
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
            map[i][j] = getNorm(bcs.getKernels()[0].getCentroid(), acs.getMean(), acs.getVar())
 
    return map

def getAPRF(tfpnlist) : 
    '''get accuracy, precision, recall, F_1 measure
    listing by data and data composed on T/F, P/N
    '''
    tp, fp, tn, fn = 0, 0, 0, 0
    for tf, pn in tfpnlist : 
        if tf is True and pn is True : 
            tp += 1
        elif tf is False and pn is True : 
            fp += 1
        elif tf is True and pn is False : 
            tn += 1
        elif tf is False and pn is False : 
            fn += 1
    tp, fp, tn, fn = float(tp), float(fp), float(tn), float(fn)
    acc = (tp+tn)/(tp+fp+tn+fn)
    pre = tp/(tp+fp)
    rec = tp/(tp+fn)
    fme = (2 * pre * rec) / (pre + rec)

    return acc, pre, rec, fme

def svcfactory(data, name) : 
    mod = SVC(kernel='linear')
    mod.fit(data, name)
    mod.fit([[0], [1]],[0, 1])
    return mod