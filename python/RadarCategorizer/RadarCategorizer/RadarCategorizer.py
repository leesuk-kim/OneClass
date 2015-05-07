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

CATArr_INDEX_NAME = 0
CATArr_INDEX_TRAIN = 1
CATArr_INDEX_TEST = 2
CATArr_INDEX_CAT = 1
class RadarCategorizer : 
    '''
    rader categorizer
    '''
    def __init__(self, ) : 
        self._CategoryArr = []
        '''raw matrix'''
        self._CatLen = 0
        self._TrainingRatio = 90
        self._CatDim = 1
        '''
training ratio is the ratio for how many data would be taken to training data.
this variable has range 1 to 99. 99 means size of training = 99%, test = 1% on raw data
        '''
        self.isCentroid = True
        pass

    TESTDATA_SOURCE = 'train'
    '''
    if train, take testdata from train data with training ratio
    else if test, take testdata from test data
    '''

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
            self._CatLen += 1
        else : 
            cat[1].append(seq)
        pass
    def appendTrain(self, matrix) : 
        try : 
            self.appendTrain(matrix[x] for x in range(len(matrix)))
        except TypeError : 
            print 'please input a vector.'
            pass
        pass

    def getCategory(self) : 
        return self._CategoryArr
    
    def setTrainingRatio(self, ratio) : 
        if ratio > 99 : 
            print 'wring range. 1 to 99 integer only.'
        else : 
            self._TrainingRatio = ratio
        pass
    def getTrainingRatio(self) : 
        return self._TrainingRatio

    def setCtrdLen(self, len) : 
        self._ctrdLen = len

    CAT_DIM = 0
    def train(self) : 
        print 'train'
        
        RadarCategorizer.CAT_DIM = len(self._CategoryArr[0][CATArr_INDEX_TRAIN][0])
        
        if self.TESTDATA_SOURCE is 'train' : 
            #split data and batch on cls space
            for cat in self._CategoryArr : 
                datalen = len(cat[CATArr_INDEX_TRAIN])
                trlen = int(datalen * self._TrainingRatio * 0.01)
                trainArr = cat[CATArr_INDEX_TRAIN][:trlen]
                testArr = cat[CATArr_INDEX_TRAIN][trlen:]
                cat.pop(CATArr_INDEX_TRAIN)
                cat.append(RadarCategorizer.RadarCategory(cat[CATArr_INDEX_NAME], trainArr, testArr))

            #set Centroid
            for cat in self._CategoryArr : 
                cat[CATArr_INDEX_CAT].setCentroid()
            pass
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

            subres = [0, 0]
            for vlpos in vldata : 
                minnorm = 1. * self.CAT_DIM
                minname = 'name'
                for ctrd in ctrdmap : 
                    euclidean = npla.norm(vlpos-ctrd[0]._Pos)
                    print '%s\t%s\t%lf'% (cat[CATArr_INDEX_NAME], ctrd[0]._Category._Name, euclidean)
                    if minnorm > euclidean : 
                        minnorm = euclidean
                        minname = ctrd[0]._Category._Name
                        
                
                if cat[CATArr_INDEX_NAME] is minname : 
                    subres[0] += 1
                else : 
                    subres[1] += 1
                resname.append([cat[CATArr_INDEX_NAME], minname])
            res.append(subres)
        pass

    class RadarCategory : 
        '''
        radar category
        '''
        def __init__(self, name, traindata, testdata) : 
            self._Name = name
            self._TrainArr = np.array(traindata)
            self._TestArr = np.array(testdata)
            self._CentroidArr = []
            pass

        def setCentroid(self) : 
            '''
            get centroid
            in Radar Categorizer, we take ONLY ONE centroid.
            '''
            self._trMean = self._TrainArr.mean(axis=0)
            self._trVar = self._TrainArr.var(axis=0, ddof=1)
            self._CentroidArr = []
            self._CentroidArr.append(RadarCategorizer.RadarCategory.CategoryKernel(self._trMean, self))
            pass

        def getTrainData(self) : 
            return self._TrainArr

        def getValidData(self) : 
            return self._TestArr

        def getCentroidList(self) : 
            return self._CentroidArr

        class CategoryKernel : 
            def __init__(self, ctrd_pos, rCat) : 
                self._Pos = ctrd_pos
                self._Category = rCat
                #self._trNorm = trNorm = [npla.norm(ctrd_pos - x) for x in rCat.getTrainData()]
                #self._lentr = lentr = len(trNorm)
                #self._trNorm_mean = trNorm_mean = np.mean(trNorm)
                #self._trNorm_var = trNorm_var = np.var(trNorm, ddof = 1)
                #featureScaling = trNorm
                #fsmax, fsmin = max(featureScaling), min(featureScaling)
                #featureScaling.sort()
                #self._trNorm_fs = trNorm_fs = [(x - fsmin) / (fsmax - fsmin) for x  in featureScaling]
                #
                #y_pval, y_d, y_sigma, y_beta_a, y_beta_b , ecdf = self.getKernel()
                #y = self.setKernel(y_sigma)
                ##ecdf = [float(x) / float(lentr) for x in range(1, lentr + 1)]
                #betaCDF = beta.cdf(y, y_beta_a, y_beta_b)
                #betaPDF = beta.pdf(y, y_beta_a, y_beta_b)
                #
                ##lkp.plotPDF(y, rCat._Name)
                ##lkp.plotCDF(y, rCat._Name)
                #lkp.plotKStest(y, ecdf, betaCDF, y_beta_a, y_beta_b, y_pval, rCat._Name)
                ##lkp.plotBetaPDF(trNorm_fs_Beta_a, trNorm_fs_Beta_b, rCat._Name)
                ##lkp.plotBetaCDF(trNorm_fs_Beta_a, trNorm_fs_Beta_b, rCat._Name)
                pass

            def setKernel(self, sigma) :
                y = [math.exp(-1 * (x ** 2) / (2 * sigma ** 2)) for x in self._trNorm]
                return [1 - yi for yi in y]

            def getKernel(self) : 
                sigma_cddt = [2 ** (i - 4) for i in range(7)]
                nmnt = [0., 0., 0., 0., 0., 0.]
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
                    d, pval = scistats.ks_2samp(ecdf, betacdf)
                    #lkp.plotKStest(y, betacdf, bafit, bbfit)

                    if nmnt[0] < pval : 
                        nmnt = [pval, d, sigma, bafit, bbfit, ecdf]

                return nmnt

            def getCentroid(self) : 
                return self._Pos
            pass
        pass
    pass


if __name__ == '__main__' : 
    obj = RadarCategorizer()

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

    
    obj.setTrainingRatio(50)
    obj.train()
    obj.test()
    print obj.getCategory()
    pass
