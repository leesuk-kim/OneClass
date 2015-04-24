import numpy as np

class OneClass : 
    def __init__(self, filename) : 
        self.name = filename
        self.motherSample = []
        self.sample = []
        pass
    def getName(self) : return self.name
    def getMotherSample(self) : return self.motherSample
    def getSample(self) : return self.sample

    def appendCsvRow(self, csvRow) : 
        row = csvRow.split(',')

        for i in range(1, len(row)) : 
            row[i] = float(row[i])
        self.motherSample.append(row)
        pass

    def dispense(self) : 
        i = 0
        inner = []
        for row in self.motherSample : 
            if len([x for x in self.sample if row[0] in x[0]]) == 0 and len([x for x in inner if row[0] in x[0]]) == 0 : 
                if len(inner) == 100: 
                    self.sample.append(inner)
                    print 'start'
                    i += 1
                inner = []
                inner.append(row)
            else : 
                inner.append(row)
                pass
        pass

    pass

name = 'emitter'

if __name__ == '__main__' : 
    oc = None
    with open(name + '.csv', 'rb') as f : 
        oc = OneClass(name)
        for line in f.readlines() : 
            oc.appendCsvRow(line)
            pass
        pass

    oc.dispense()

    print oc.getSample()
    

    pass