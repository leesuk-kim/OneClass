
import re
import sys
import numpy as np
#import scipy.stats as stats
import math

class raex : 
    ORI_FREQ = 0
    ORI_TOA = 1
    ORI_DPW = 2
    ORI_DIM_LEN = 3
    ORI_SAMPLE_LEN = 100
    STAT_MEA = 0
    STAT_VAR = 1
    STAT_SKW = 2
    STAT_KTS = 3
    RAW_DIM_NONAME = 11

    def __init__(self, classname): 
        self.name= classname;
        '''identification name'''
        self.srclist = []
        '''original data'''
        self.appendable = True;
        '''
        flag for whether this class is appendable.
        if not appendable, appending functions don't work
        '''

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
        self.originDimSize  = 0
        pass
    
    def appendData(self, dlist) : 
        if self.appendable :     
            srcbfr = []
            for d in dlist : 
                s = re.sub(' +', ' ', d)#change 1 more whitespaces to a whitespace in the string d
                s = s[1:].split(' ')
                srcbfr.append([float(s[4]), float(s[8]), float(s[10])])#FREQ, TOA, PW
        
            self.srclist.append(srcbfr)
        pass

    #def setRawData(self) : 
    #    '''
    #    set Rawdata - 13 dimension (12 dim + 1 clsname)
    #    '''
    #    if not self.appendable : 

    #        #calculate via my shape
    #        for i in range(raex.ORI_SAMPLE_LEN) :
    #            rawRow = [self.name]
    #            rawRow.extend([0. for x in range(raex.RAW_DIM_NONAME)])
                
    #            for m in range(raex.ORI_DIM_LEN) : 
    #                k = m + 1
    #                rawRow[4 * k - 7] = self.originMean[i][m]
    #                rawRow[4 * k - 6] = self.originVar[i][m]
    #                rawRow[4 * k - 5] = self.originSkew[i][m]
    #                rawRow[4 * k - 4] = self.originKurto[i][m]
    #                rawRow[4 * k - 3] = self.originDIfMean[i][m]
    #                rawRow[4 * k - 2] = self.originDIfVar[i][m]
    #                rawRow[4 * k - 1] = self.originDIfSkew[i][m]
    #                rawRow[4 * k] = self.originDIfKurto[i][m]
    #                #rawRow[4 * k - 3] = self.originMean[i][m]
    #                #rawRow[4 * k - 2] = self.originVar[i][m]
    #                #rawRow[4 * k - 1] = self.originSkew[i][m]
    #                #rawRow[4 * k] = self.originKurto[i][m]

    #            self.rawdata.append(rawRow)
    #    pass
    
    def setOriginStatistics(self) :
        if not self.appendable : 
            dimlen = raex.ORI_DIM_LEN

            for sample in self.srclist : 
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
        for i in range(len(self.srclist)) :
            toa = [arr[1] for arr in self.srclist[i]]
            dtoa = diff(toa, True)

            for j in range(len(self.srclist[i]) - 1, 0, -1) : 
                self.srclist[i][j][1] = dtoa[j]

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

    def setRaw(self) : 
        raw = []
        for splist in self.srclist : 
            features = []
            for dim in splist : 
                features.extend(statfeatures(dim))
            raw.append(features)

        self.rawdata = raw

    def appenddiff(self) : 
        for splist in self.srclist : 
            pass
        pass

    def close(self) : 
        self.appendable = False;
        #self.setDTOA()
        for i, v in enumerate(self.srclist) : 
            self.srclist[i] = zip(*v)#transpose matrix!!!!!!

        self.setRaw()
        self.appenddiff(self.srclist)
    pass

def setFeatureScaling(cls) : 
    minList = [sys.float_info.max for i in range(raex.RAW_DIM_NONAME)]
    maxList = [0 for i in range(raex.RAW_DIM_NONAME)]
    denom = [0 for i in range(raex.RAW_DIM_NONAME)]
    sp = 0.
    for i in range(0, raex.RAW_DIM_NONAME) : 
        for obj in cls : 
            for sample in obj.getRawData() : 
                sp = sample[i+1]
                if maxList[i] < sp : 
                    maxList[i] = sp
                elif minList[i] > sp : 
                    minList[i] = sp
        print 'min=[%lf], max=[%lf]' % (minList[i], maxList[i])
        denom[i] = maxList[i] - minList[i]

    for i in range(0, raex.RAW_DIM_NONAME) : 
        idx = i + 1
        for j in range(len(cls)) : 
            samplebook = cls[j].getRawData()
            for k in range(len(samplebook)) : 
                sp = (samplebook[k][idx] - minList[i]) / denom[i]
                cls[j].replaceRawData(k, idx, sp)
    pass

def diff(list, zindex = False) : 
    '''difference sequencial values in the list
    param:
    list - like array or list.
    zindex - boolean for start at zero or one.
    if zindex is false, it start index of n+1 and return list whose size is n-1
    if zindex is true, it start index of n and return list whose size is n
    '''
    sub = list[0 if zindex else 1:]
    if zindex :
        list.insert(0, 0)
    list = list[:-1]
    dif = [x2 - x1 for (x1, x2) in zip(list, sub)]

    return dif

def statfeatures(list, moments = 'mvsk') : 
    '''
    moment - m/v/s/k. passive is mv.
    if mv, it returns m and v, or if mvk, it returns m, v, and k.
    '''
    sflist = []
    ll = float(len(list))
    lls = ll - 1
    dll,  dlls = 1 / ll, 1 / lls

    if 'm' in moments: 
        m = sum(list) / ll
        sflist.append(m)
        
    if 'v' in moments : 
        v = sum([dlls * (x - m) ** 2 for x in list])
        sflist.append(v)

    if 's' in moments : 
        m3 = (dll * sum([dlls * (x - m) ** 3 for x in list])) 
        b1 = m3 / (v ** 1.5)
        sflist.append(b1)

    if 'k' in moments : 
        vsquare = (sum([dlls * (x - m) ** 2 for x in list])) ** 2
        m4 = (dll * sum([dlls * (x - m) ** 4 for x in list]))
        g2 = m4 / vsquare - 3
        sflist.append(g2)

    return sflist