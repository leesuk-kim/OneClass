import os
import re
import sys
import numpy as np

class CM : 
    ORI_FREQ = 0
    ORI_TOA = 1
    ORI_DPW = 2
    ORI_DIM_LEN = 3
    ORI_SAMPLE_LEN = 100
    STAT_MEA = 0
    STAT_VAR = 1
    STAT_SKW = 2
    STAT_KTS = 3
    RAW_DIM = 13
    RAW_DIM_NONAME = RAW_DIM - 1

    def __init__(self, classname): 
        self.name= classname;
        '''identification name'''
        self.originData = []
        '''original data'''
        self.originDev = []
        '''original deviation'''
        self.originMean = []
        '''original mean'''
        self.originVar = []
        '''original variance'''
        self.originSkew = []
        '''original skewness'''
        self.originKurto = []
        '''original kurtosis'''
        self.dimSize = 11
        '''length of dimension of raw data'''
        self.rawdata = []
        '''list of raw data'''
        self.mean = []
        '''list of mean of raw data'''
        self.appendable = True;
        '''
        flag for whether this class is appendable.
        if not appendable, some function can't work
        '''
        self.originDimSize  = 0
        pass
    
    def appendData(self, dlist) : 
        if self.appendable :     
            origin = []
            for d in dlist : 
                s = re.sub(' +', ' ', d)#change 1 more whitespaces to a whitespace in the string d
                s = s[1:].split(' ')
                origin.append([float(s[4]), float(s[8]), float(s[10])])#FREQ, dTOA, PW
        
            self.originData.append(origin)
        pass

    def setRawData(self) : 
        '''
        set Rawdata - 13 dimension (12 dim + 1 clsname)
        '''
        if not self.appendable : 

            #calculate via my shape
            for i in range(CM.ORI_SAMPLE_LEN) :
                rawRow = [self.name]
                rawRow.extend([0. for x in range(CM.RAW_DIM_NONAME)])
                
                #temp = [self.originMean[i], self.originVar[i], self.originSkew[i], self.originKurto[i]]
                for m in range(CM.ORI_DIM_LEN) : 
                    k = m + 1
                    rawRow[4 * k - 3] = self.originMean[i][m]
                    rawRow[4 * k - 2] = self.originVar[i][m]
                    rawRow[4 * k - 1] = self.originSkew[i][m]
                    rawRow[4 * k] = self.originKurto[i][m]

                self.rawdata.append(rawRow)
        pass
    
    def setOriginStatistics(self) :         
        if not self.appendable : 
            dimlen = CM.ORI_DIM_LEN

            for sample in self.originData : 
                #get mean
                dimMean = [0. for m in range(dimlen)]
                sprLen = len(sample)
                for i in range(dimlen) : 
                    for sprow in sample : 
                        dimMean[i] += sprow[i]

                    dimMean[i] /= sprLen;
                self.originMean.append(dimMean)
                
                #get deviation
                dimDev = [[0. for m in range(dimlen)] for n in range(sprLen)]
                for sprow in range(sprLen) : 
                    for i in range(dimlen) : 
                        dimDev[sprow][i] = sample[sprow][i] - dimMean[i]
                    pass
                self.originDev.append(dimDev)

                #get variance
                dimVar = [0. for m in range(dimlen)]
                for i in range(dimlen) : 
                    for sprow in range(sprLen) : 
                        dimVar[i] += dimDev[sprow][i] ** 2
                        pass
                    dimVar[i] /= sprLen - 1
                self.originVar.append(dimVar)

                #get skewness and kurtosis
                dimSkew = [0. for m in range(dimlen)]
                dimKurto = [0. for m in range(dimlen)]
                for i in range(dimlen) : 
                    for sprow in range(sprLen) : 
                        dimSkew[i] += dimDev[sprow][i] ** 3
                        dimKurto[i] += dimDev[sprow][i] ** 4
                    dimSkew[i] /= sprLen * (dimVar[i] ** 1.5)
                    dimKurto[i] /= sprLen * (dimVar[i] ** 2)
                    dimKurto[i] -= 3
                self.originSkew.append(dimSkew)
                self.originKurto.append(dimKurto)
                pass
        pass

    def setDTOA(self) : 
        for i in range(len(self.originData)) : 
            for j in range(len(self.originData[i]) - 1, 0, -1) : 
                self.originData[i][j][1] = self.originData[i][j][1] - self.originData[i][j - 1][1]
                pass
            pass
        pass

    def getVar(self) :
        if self.appendable : 
            print 'The variance of Raw data is not ready'
            pass
        return self.var

    def getRawData(self) : 
        if self.appendable : 
            print "The Raw data is not ready"
            pass
        return self.rawdata

    def replaceRawData(self, row, col, rep) : 
        self.rawdata[row][col] = rep
        pass

    def getName(self) : 
        return self.name

    def closeClass(self) : 
        self.appendable = False;
        self.setDTOA()
        self.setOriginStatistics()
        self.setRawData()
    pass

def setFeatureScaling(cls) : 
    minList = [sys.float_info.max for i in range(CM.RAW_DIM_NONAME)]
    maxList = [0 for i in range(CM.RAW_DIM_NONAME)]
    denom = [0 for i in range(CM.RAW_DIM_NONAME)]
    sp = 0.
    for i in range(0, CM.RAW_DIM_NONAME) : 
        for obj in cls : 
            for sample in obj.getRawData() : 
                sp = sample[i+1]
                if maxList[i] < sp : 
                    maxList[i] = sp
                elif minList[i] > sp : 
                    minList[i] = sp
        print 'min=[%lf], max=[%lf]' % (minList[i], maxList[i])
        denom[i] = maxList[i] - minList[i]

    for i in range(0, CM.RAW_DIM_NONAME) : 
        idx = i + 1
        for j in range(len(cls)) : 
            samplebook = cls[j].getRawData()
            for k in range(len(samplebook)) : 
                sp = (samplebook[k][idx] - minList[i]) / denom[i]
                cls[j].replaceRawData(k, idx, sp)
    pass

if __name__ == "__main__" : 
    clist = []
    for dirname, dirnames, filenames in os.walk('.'):
        # print path to all subdirectories first.
        for subdirname in dirnames:
            os.path.join(dirname, subdirname)
            #print(os.path.join(dirname, subdirname))

        if 'data' in dirname :
            if filenames : 
                clsname = dirname.split('\\')
                cm = CM('_'.join(clsname[2:]))
                # print path to all filenames in subdirname.
                for filename in filenames:
                    if '.txt' in filename and not '%' in filename:
                        os.path.join(dirname, filename)
                        #print os.path.join(dirname, filename)
                        dlist = []
                        file = open(dirname + '\\' + filename, 'r')
                        file.readline(), file.readline(), file.readline()
                        lines = file.readlines()
                        for line in lines : 
                            dlist.append(line)
                        cm.appendData(dlist)
                        #cm.appendData(dlist, dlist.count)
                cm.closeClass()
                #data = cm.getMean()
                #print data
                clist.append(cm)
                print 'append %s' % cm.getName()

    #feature-scaling
    setFeatureScaling(clist)
    #export Data
    with open('merge.csv', 'wb') as fm : 
        for c in clist : 
            with open(c.getName() + '.csv', 'wb') as f : 
                for data in c.getRawData() : 
                    f.write('%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n'%(data[0], repr(data[1]), repr(data[2]), repr(data[3]), repr(data[4]), repr(data[5]), repr(data[6]), repr(data[7]), repr(data[8]), repr(data[9]), repr(data[10]), repr(data[11]), repr(data[12])))
                    fm.write('%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n'%(data[0], repr(data[1]), repr(data[2]), repr(data[3]), repr(data[4]), repr(data[5]), repr(data[6]), repr(data[7]), repr(data[8]), repr(data[9]), repr(data[10]), repr(data[11]), repr(data[12])))
                    pass
                print 'file write : ' + c.getName() + '.csv'

