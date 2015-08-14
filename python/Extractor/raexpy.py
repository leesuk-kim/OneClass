import re


class RadarExtractor:

    def __init__(self, classname): 
        self.name = classname
        """identification name"""
        self.srclist = []
        """original data"""
        self._appendable = True
        """
        flag for whether this class is appendable.
        if not appendable, appending functions don't work
        """
        self.rawdata = []
        pass
    
    def append_data(self, dlist):
        if self._appendable:     
            srcbfr = []
            for d in dlist: 
                s = re.sub(' +', ' ', d)  # change 1 more whitespaces to a whitespace in the string d
                s = s[1:].split(' ')
                # srcbfr.append([float(s[4]), float(s[11]), float(s[10])])  # FREQ, dTOA, PW
                srcbfr.append([float(s[4]), float(s[8]), float(s[10])])  # FREQ, TOA, PW
        
            self.srclist.append(srcbfr)
        pass

    def set_raw(self):
        raw = []
        for splist in self.srclist: 
            features = []
            for dim in splist: 
                features.extend(statfeatures(dim))
            raw.append(features)

        self.rawdata = raw

    def __diff__(self): 
        """
        CUSTERMIZE FOR NEW PROJECT
        """
        for splist in self.srclist:
            diflist = [] 
            for idx, dim in enumerate(splist): 
                dfrc = diff(dim)
                diflist.append(dfrc)
            splist.extend(diflist)
            pass
        pass

    def close(self): 
        self._appendable = False

        for i, v in enumerate(self.srclist): 
            self.srclist[i] = list(zip(*v))  # transpose matrix!!!!!!

        # for i, toa in enumerate(self.srclist): 
        #     dtoa = diff(toa[1], zindex=True)
        #     toa.pop(1)
        #     toa.insert(1, dtoa)

        # self.__diff__()
        self.set_raw()
        pass
    pass


def diff(arr, zindex=False, absolute=False):
    """difference sequencial values in the list
    param:
    list - like array or list.
    zindex - boolean for start at zero or one.
    if zindex is false, it start index of n+1
    if zindex is true, it start index of n
    if absolute is true, it takes list on absolute
    if absolute is fale, it takes list on raw
    """
    sub = arr[0 if zindex else 1:]
    sub2 = [x for x in arr]
    if zindex:
        sub2.insert(0, 0)
    sub2 = sub2[:-1]
    dif = [x2 - x1 if not absolute else abs(x2 - x1) for (x1, x2) in list(zip(sub2, sub))]

    if not zindex: 
        dif.append(sum(dif) / float(len(sub2)))
    return dif


def statfeatures(arr, moments="mvsk"):
    """
    moment - m/v/s/k. passive is mv.
    if mv, it returns m and v, or if mvk, it returns m, v, and k.
    """
    sfarr = []
    ll = float(len(arr))
    lls = ll - 1
    dll, dlls = 1 / ll, 1 / lls
    v, m, = 0., 0.

    if 'm' in moments: 
        m = sum(arr) / ll
        sfarr.append(m)
        
    if 'v' in moments: 
        v = sum([dlls * (x - m) ** 2 for x in arr])
        sfarr.append(v)

    if 's' in moments: 
        m3 = (dll * sum([dlls * (x - m) ** 3 for x in arr])) 
        b1 = 0 if v == 0 else m3 / (v ** 1.5)
        sfarr.append(b1)

    if 'k' in moments: 
        vsquare = (sum([dlls * (x - m) ** 2 for x in arr])) ** 2
        m4 = (dll * sum([dlls * (x - m) ** 4 for x in arr]))
        g2 = 0 if vsquare == 0 else m4 / vsquare - 3
        sfarr.append(g2)

    return sfarr


def featurescaling(clist: list):
    """AMP는 올바르게 작동하지 않습니다.
    """
    """
    cslist: class sample list
    dslist: dimensional sample list 
    cdslist: list for dimensional sample of classes
    slist: sample list
    clist: class list
    dxlist: list for max of dimension
    dnlist: list for min of dimension 
    dflist: list for feature on dimension
    """
    if type(clist[0]) is not RadarExtractor: 
        print("TYPE INCORRECT")
        return 0

    cdslist = [list(zip(*cls.rawdata)) for cls in clist]  # 각 class마다 시간순으로 정리된 sample을 dimension으로 정리해서 복사
    dslist = list(zip(*cdslist))  # 각 class순으로 저장된 list를 dimension으로 정리해서 복사
    dxlist = [max([max(y) for y in x]) for x in dslist]  # 각 dimension의 최대값
    dnlist = [min([min(y) for y in x]) for x in dslist]  # 각 dimension의 최소값
    ddlist = [dx - dn for dx, dn in zip(dxlist, dnlist)]

    for cls in clist:  # each class
        frawdata = []
        for samp in cls.rawdata:  # each sample
            smpl = [(s - dn) / dd for s, dn, dd in (zip(samp, dnlist, ddlist))]

            frawdata.append(smpl)
            pass
        cls.rawdata = frawdata
        pass
