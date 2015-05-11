'''
Radar signal Categorizer
version 1.0
Designed by leesuk kim(aka. LK)
'''
import numpy as np
import numpy.linalg as npla
from scipy.stats import beta
import scipy.stats as scistats
import lkplot as lkp
import math
import ocpymath

CATArr_INDEX_NAME = 0
CATArr_INDEX_TRAIN = 1
CATArr_INDEX_TEST = 2
CATArr_INDEX_CAT = 1
class cpclassipy : 
    '''
    Class Probability ClassiPier
    '''
    def __init__(self) : 
        self._CategoryArr = []
        '''raw matrix'''
        self._ClsVol = 0
        self._KnVol = 2
        self.VALID_SRC = 'train'
        self._CatDim = 1
        self._TrRatio = 90
        '''
training ratio is the ratio for how many data would be taken to training data.
this variable has range 1 to 99. 99 means size of training = 99%, test = 1% on raw data
        '''
        pass

    def getCategory(self) : 
        return self._CategoryArr

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
    
    def setTrRatio(self, ratio) : 
        '''put percentage in parameter.
        i.e. 90% --> 90, 50%-->50, 20.2% -> 20.2
        '''
        if ratio >= 100 or ratio <= 0 : 
            print 'wrong range. availlable from >0 to <100 .'
        else : 
            self._TrRatio = ratio

    def getTrRatio(self) : 
        return self._TrRatio

    def setKnVol(self, knvol) : 
        '''if it has fixed number of kernel, then put number in.
        '''
        self._KnVol = knvol
    
    VALID_SRC = 'train'
    def setValidSrc(self, src = 'test') : 
        '''
    if train, take testdata from train data with training ratio
    if 'test', take testdata from test data
        '''
        VALID_SRC = src
    def getValidSrc(self) : 
        return VALID_SRC

    CAT_DIM = 0

    def getCategory(self, idx) : 
        return self._CategoryArr[idx]

    def train(self) : 
        print 'train'
        
        CAT_DIM = len(self.getCategory(0)[CATArr_INDEX_TRAIN][0])
        
        if self.VALID_SRC is 'train' : 
            #split data and batch on cls space
            for cat in self._CategoryArr : 
                datalen = len(cat[CATArr_INDEX_TRAIN])
                trlen = int(datalen * self._TrRatio * 0.01)
                trainArr = cat[CATArr_INDEX_TRAIN][:trlen]
                testArr = cat[CATArr_INDEX_TRAIN][trlen:]
                cat.pop(CATArr_INDEX_TRAIN)
                cat.append(cpclassipy.clsobj(cat[CATArr_INDEX_NAME], trainArr, testArr, self._KnVol))

            #set Centroid
            for cat in self._CategoryArr : 
                cat[CATArr_INDEX_CAT].setKernel()
        pass

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

    class clsobj : 
        '''
        radar category
        '''
        def __init__(self, name, traindata, testdata, knvol) : 
            self._Name = name
            self._TrainArr = np.array(traindata)
            self._TestArr = np.array(testdata)
            self._trMean = self._TrainArr.mean(axis=0)
            self._trVar = self._TrainArr.var(axis=0, ddof=1)
            self._trDev = self._TrainArr.std(axis=0, ddof=1)
            self._CentroidArr = []
            self._KernelSize = knvol
            self._dim = len(testdata[0])
            pass

        def getName(self):
            return self._Name
        def getMean(self) : 
            return self._trMean
        def getVar(self) : 
            return self._trVar
        def getDev(self) : 
            return self._trDev

        def setKernelSize(self, size) : 
            self._KernelSize = size
            pass

        def setKernel(self) : 
            '''
            set centroid
            get Centroid position and set that pos. on each kernels
            in Radar Categorizer, we take ONLY ONE centroid.
            '''
            self.initCtrdPos()
            self._kernelPos = []

            self._CentroidArr = []

            for knl in range(self._KernelSize) : 
                self._CentroidArr.append(cpclassipy.clsobj.krnl(self._trMean, self))
            pass

        def initCtrdPos(self) : 
            pos = []
            pass

        def getTrainData(self) : 
            return self._TrainArr

        def getValidData(self) : 
            return self._TestArr

        def getCentroidList(self) : 
            return self._CentroidArr

        class krnl : 
            def __init__(self, ctrd_pos, rCat) : 
                self._Pos = ctrd_pos
                self._Category = rCat
                self._trNorm = trNorm = [npla.norm(ctrd_pos - x) for x in rCat.getTrainData()]
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
        pass
    pass


if __name__ == '__main__' : 
    obj = cpclassipy()
    with open('merge.csv') as f : 
        for line in f.readlines() : 
            line = line.split(',')
            row = []
            for attr in line : 
                try : 
                    attr = float(attr)
                except ValueError : 
                    pass
                row.extend([attr])
            obj.appendTrainRow(row)

    
    obj.setTrRatio(50)
    obj.train()
    res, resname = obj.test()
    for ult in res : 
        print ult
    for ultname in resname : 
        print ultname
    pass
