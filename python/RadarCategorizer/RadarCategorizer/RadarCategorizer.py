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
            self._ClsSpaceArr = []

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
            self._CentroidArr.append(RadarCategorizer.RadarCategory.CategoryCentroid(self._trMean, self))
            pass

        def getTrainData(self) : 
            return self._TrainArr
        def getTestData(self) : 
            return self._TestArr

        class CategoryCentroid : 
            def __init__(self, ctrd_pos, rCat) : 
                self._Pos = ctrd_pos
                self._Category = rCat
                self._trNorm = trNorm = [npla.norm(ctrd_pos - x) for x in rCat.getTrainData()]
                self._ttNorm = ttNorm = [npla.norm(ctrd_pos - x) for x in rCat.getTestData()]
                self._trNorm_mean = trNorm_mean = np.mean(trNorm)
                self._trNorm_var = trNorm_var = np.var(trNorm, ddof = 1)
                featureScaling = trNorm
                featureScaling.sort()
                self._trNorm_fs = trNorm_fs = [(x - featureScaling[0]) / (featureScaling[-1] - featureScaling[0]) for x  in featureScaling]
                self._trNorm_fs_mean = trNorm_fs_mean = np.mean(trNorm_fs)
                self._trNorm_fs_var = trNorm_fs_var = np.var(trNorm_fs, ddof = 1)
                self._trNorm_fs_betaA = trNorm_fs_Beta_a = trNorm_fs_mean ** 2 * ((1 - trNorm_fs_mean) / trNorm_fs_var - 1 / trNorm_fs_mean)
                self._trNorm_fs_betaB = trNorm_fs_Beta_b = trNorm_fs_Beta_a * (1 - trNorm_fs_mean) / trNorm_fs_mean

                lkp.plotPDF(trNorm_fs, rCat._Name)
                lkp.plotCDF(trNorm_fs, rCat._Name)
                lkp.plotBetaPDF(trNorm_fs_Beta_a, trNorm_fs_Beta_b, rCat._Name)
                lkp.plotBetaCDF(trNorm_fs_Beta_a, trNorm_fs_Beta_b, rCat._Name)
                pass

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

    
    obj.setTrainingRatio(90)
    obj.train()
    obj.test()
    print obj.getCategory()
    pass
