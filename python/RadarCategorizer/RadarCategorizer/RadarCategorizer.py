'''
Radar signal Categorizer
version 1.0
Designed by leesuk kim(aka. LK)
'''

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
    def appendRawRow(self, row) : 
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
    
    def appendRaw(self, matrix) : 
        try : 
            self.appendRaw(matrix[x] for x in range(len(matrix)))
        except TypeError : 
            print 'please input a vector.'
            pass
        pass

    def getRaw(self) : 
        str = ''
        for cat in self._CategoryList : 
            str += '[category name : %s]\n' % cat[0]
            for i, catRaw in enumerate(cat[1]) : 
                str += '[%d]%lf\n'%(i, catRaw[0])
        return str
    
    def setTrainingRatio(self, ratio) : 
        if ratio > 99 : 
            print 'wring range. 1 to 99 integer only.'
        else : 
            self._TrainingRatio = ratio
        pass
    def getTrainingRatio(self) : 
        return self._TrainingRatio

    def setCentriodMax(self, max) : 
        self._CtrdLenMax= max
        pass

    def train(self, useCentroid = True, linear_discrimininant = False) : 
        self.isCentroid = useCentroid
        #set centroid
        if self.isCentroid : 
            pass
        pass

    def test(self) : 
        pass

    class RadarCategory : 
        '''
        radar category
        '''
        def __init__(self, name) : 
            self._Name = name
            self._Raw = []
            self._Beta_a = 0.
            self._Beta_b = 0.
            self._CentroidList = []
            self._Pdf = []
            self._fsPdf = []
            self._BetaDist_A = []
            self._BetaDist_B = []

            pass

        def setRawRow(self, row) : 
            self._Raw.extend([row])

        def setRaw(self, matrix) : 
            self.setRawRow(row for row in matrix )

        def getTrainingData(self, ratio) : 
            return self._Raw[:round(0.01 * ratio * len(self._Raw))]

        def getTestData(self, ratio) : 
            return self._Raw[round(0.01 * ratio * len(self._Raw)):]

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
            obj.appendRawRow(row)

    obj.setTrainingRatio(90)
    obj.setCentriodMax()
    obj.train()
    obj.test()
    print obj.getRaw()
    pass
