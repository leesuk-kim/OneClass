import re

class raex : 

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

    def getRawData(self) : 
        if self.appendable : 
            print "The Raw data is not ready"
            pass
        return self.rawdata

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
        '''
        CUSTERMIZE FOR NEW PROJECT
        '''
        for splist in self.srclist :
            diflist = [] 
            for idx, dim in enumerate(splist) : 
                dfrc = diff(dim, zindex = False if idx != 1 else True)
                diflist.append(dfrc)
            splist.extend(diflist)
            pass
        pass

    def close(self) : 
        self.appendable = False;

        for i, v in enumerate(self.srclist) : 
            self.srclist[i] = zip(*v)#transpose matrix!!!!!!

        self.appenddiff()
        self.setRaw()
    pass

def diff(list, zindex = False, absolute = False) : 
    '''difference sequencial values in the list
    param:
    list - like array or list.
    zindex - boolean for start at zero or one.
    if zindex is false, it start index of n+1
    if zindex is true, it start index of n
    if absolute is true, it takes list on absolute
    if absolute is fale, it takes list on raw
    '''
    sub = list[0 if zindex else 1:]
    sub2 = [x for x in list]
    if zindex :
        sub2.insert(0, 0)
    sub2 = sub2[:-1]
    dif = [x2 - x1 if not absolute else abs(x2 - x1) for (x1, x2) in zip(sub2, sub)]

    if not zindex : 
        dif.append(sum(dif) / float(len(sub2)))
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

#junk comment for commit