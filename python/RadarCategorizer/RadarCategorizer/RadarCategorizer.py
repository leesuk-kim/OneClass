'''
Radar signal Categorizer
version 1.0
Designed by leesuk kim(aka. LK)
'''
CATLIST_INDEX_NAME = 0
CATLIST_INDEX_TRAIN = 1
CATLIST_INDEX_TEST = 2
class RadarCategorizer : 
    '''
    rader categorizer
    '''
    def __init__(self, ) : 
        self._CategoryList = []
        '''raw matrix'''
        self._CatLen = 0
        self._TrainingRatio = 0
        '''
training ratio is the ratio for how many data would be taken to training data.
this variable has range 1 to 99. 99 means size of training = 99%, test = 1% on raw data
        '''
        self._CtrdLenMax = 1
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

        cat = next((rrow for i, rrow in enumerate(self._CategoryList) if name in rrow), False)
        if not cat : 
            print 'create new category : %s' % name
            self._CategoryList.append([name, [seq]])
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
        return self._CategoryList
    
    def setTrainingRatio(self, ratio) : 
        if ratio > 99 : 
            print 'wring range. 1 to 99 integer only.'
        else : 
            self._TrainingRatio = ratio
        pass
    def getTrainingRatio(self) : 
        return self._TrainingRatio

    def train(self) : 
        print 'train'
        if self.TESTDATA_SOURCE is 'train' : 
            self._ClsSpaceList = []
            print 'fsda;kgnsglksnagslk'
            #split data
            for cat in self._CategoryList : 
                datalen = len(cat[CATLIST_INDEX_TRAIN])
                trlen = int(datalen * 0.90)
                trainlist = cat[CATLIST_INDEX_TRAIN][:trlen]
                testlist = cat[CATLIST_INDEX_TRAIN][trlen:]
                cat.pop(CATLIST_INDEX_TRAIN)
                #cat.append(trainlist)
                #cat.append(testlist)
                cat.append(RadarCategorizer.RadarCategory(cat[CATLIST_INDEX_NAME], trainlist, testlist))

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
            self._TrainList = traindata
            self._TestList = testdata
            self._Beta_a = 0.
            self._Beta_b = 0.
            self._CentroidList = []
            self._Pdf = []
            self._fsPdf = []
            self._BetaDist_A = []
            self._BetaDist_B = []

            pass

        class CategoryCentroid : 
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
