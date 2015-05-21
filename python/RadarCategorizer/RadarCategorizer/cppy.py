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
import lkplot as lkp
import math
import ocpymath
from sklearn import svm
from sklearn import metrics


CATArr_INDEX_NAME = 0
CATArr_INDEX_TRAIN = 1
CATArr_INDEX_TEST = 2
CATArr_INDEX_CAT = 1

class cpon : 
    '''
    Class Probability Output Network
    '''
    def __init__(self, fold = 2) : 
        self._TimeStamp = int(time.time())
        self._CategoryArr = []
        '''raw matrix'''
        self._ClsVol = 0
        self._CatDim = 1
        self._cslist = []
        self._fmax = fold
        self._kmax = 1
        pass

    def getcslist(self) : 
        return self._cslist

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
        
    def getKnVol(self):
        return self._kmax
    def setKnVol(self, knvol) : 
        '''if it has fixed number of kernel, then put number in.
        if grow is true, it makes kernel volmes most effective.
        '''
        self._kmax = knvol

    def getcslist(self, idx) : 
        return self._cslist[idx]

    def Learn(self) : 
        print 'learning'

        fsblist = []
        for fold in range(self._fmax) : 
            print 'fold#%d' % fold
            for cs in self._cslist : 
                cs.onFolding(fold)
                cs.onTraining()
                
            foldsb = self.onTesting(fold)
            fsblist.append(scoring(foldsb))
        return fsblist

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
            lkp.plotoar(fn, dmap, i)
            
        return dmlist

    def learnSVM(self) : 
        print 'learning SVM'

        fsblist = []
        for fold in range(self._fmax) : 
            self._svm = svm.LinearSVC()
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

            self._svm.fit(fitdatalist, fitnamelist)#training
            foldsb = self.onSVMTesting(fold)#testing
            fsblist.append(foldsb)
        return fsblist
        
        pass

    def onSVMTesting(self, fold) : 
        plist = []
        for i, tcs in enumerate(self._cslist) : 
            for vd in tcs.getValidData() : 
                pred = self._svm.predict(vd)
                plist.append([tcs.getName(), pred[0]])
            #acc = self._svm.score(tcs.getValidData(), [tcs.getName() for x in tcs.getValidData()])
            pass
            #fn = 'svm#%d%s' % (fold + 1, tcs.getName())
            #lkp.plotoar(fn, dmap, i)
        
        return plist

    def test(self) : 
        res = []
        ctrdmap = []
        for cat in self._CategoryArr : 
            ctrdmap.append(cat[CATArr_INDEX_CAT].getCentroidList())
        
        resname = []
        for cat in self._CategoryArr : 
            vldata = cat[CATArr_INDEX_CAT].getValidData()
            mycls = 1. * self.CAT_DIM
            resdstc, dstc = [], []

            subres = [0, 0]
            for vlpos in vldata : 
                minnorm = 1. * self.CAT_DIM
                minname = 'name'

                for ctrd in ctrdmap : 
                    euclidean = ocpymath.getPairwiseDistances(vlpos, ctrd[0]._Pos, ctrd[0]._Category.getVar())#npla.norm(vlpos-ctrd[0]._Pos)
                    #dstc.append(euclidean)
                    #print '%s\t%s\t%lf'% (cat[CATArr_INDEX_NAME], ctrd[0]._Category._Name, euclidean)
                    if minnorm > euclidean : 
                        minnorm = euclidean
                        minname = ctrd[0]._Category._Name

                    if cat[CATArr_INDEX_NAME] is ctrd[0]._Category.getName() : 
                        dstc.append(euclidean)
                    else : 
                        resdstc.append(euclidean)

                
                if cat[CATArr_INDEX_NAME] is minname : 
                    subres[0] += 1
                else : 
                    subres[1] += 1
                        
                resname.append([cat[CATArr_INDEX_NAME], minname])
            lkp.plotOaR(cat[CATArr_INDEX_NAME], dstc, resdstc)

            res.append(subres)
        return res, resname
    pass


class cspace : 
    '''
    radar category
    '''
    def __init__(self, name, features, foldmax, knvol = 1) : 
        self._Name = name
        self._FeatureList = features
        self._FeatureVol = len(features)
        self._FoldValidVol = int(float(self._FeatureVol) / float(foldmax))
        self._foldTrainVol = self._FeatureVol - self._FoldValidVol
        #self._trMean = self._TrainArr.mean(axis=0)
        #self._trVar = self._TrainArr.var(axis=0, ddof=1)
        #self._trDev = self._TrainArr.std(axis=0, ddof=1)
        #self._CentroidArr = []
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
        aindex = cnt * self._FoldValidVol
        bindex = aindex + self._FoldValidVol
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
                v /= self._foldTrainVol - 1
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
    pass

class kspace : 
    def __init__(self, ctrd_pos, trd, name='noname') : 
        self._Pos = ctrd_pos
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
                
        #lkp.plotPDF(y, rCat._Name)
        #lkp.plotCDF(y, rCat._Name)
        #lkp.plotKStest(y, ecdf, betaCDF, y_beta_a, y_beta_b, y_pval, rCat._Name)
        #lkp.plotBetaPDF(trNorm_fs_Beta_a, trNorm_fs_Beta_b, rCat._Name)
        #lkp.plotBetaCDF(trNorm_fs_Beta_a, trNorm_fs_Beta_b, rCat._Name)
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
        lkp.plotKStest(y, nmnt[5], nmnt[6], nmnt[3], nmnt[4], nmnt[0], self._Category._Name)

        return nmnt

    def getCentroid(self) : 
        return self._Pos
    pass

def getNorm(row, mean, var) : 
    d = 0.
    d = reduce(lambda x, y : x + y , [(i - m) ** 2 / v for i, m, v in zip(row, mean, var)])
    #for i, m, v in zip(row, mean, var) : 
    #    d += (i - m) ** 2 / v
    return d ** 0.5

def scoring(distancemap) : 
    sb = []
    map = [zip(*tcmap) for tcmap in distancemap]
    for eclist in map : 
        ecsb = []
        for klist in eclist : 
            v = reduce(lambda x, y : x if x < y else y, min(klist))
            i = klist.index(v)
            ecsb.append(i)
        sb.append(ecsb)
        pass
    return sb