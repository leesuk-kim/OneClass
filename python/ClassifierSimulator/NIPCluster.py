__author__ = 'lk'
import math


def cluster(x: list, **kwargs):
    key = kwargs['key'] if 'key' in kwargs else lambda k: k
    diffkey = kwargs['diffkey'] if 'diffkey' in kwargs else lambda k: sum(k)
    lenx = len(x)
    if 'bins' in kwargs:
        bins = kwargs['bins']
        candibins = bins
    else:
        bins, candibins = int(round(math.log(lenx, 10), 0)), (int(math.log(lenx)))
    clemberlist, histogram, clember = [], [], Clember(0)

    y = [[key(a), a] for a in x]
    y.sort(key=lambda x: x[0])
    x = [a[1] for a in y]
    y = [a[0] for a in y]
    yn, yx = y[0], y[-1]
    boxbound = (yx - yn) / candibins
    box = yn + boxbound
    for a, b in list(zip(x, y)):
        if b > box:
            histogram.append(clember.close)
            clember = Clember(len(histogram))
            box += boxbound
        clember.regist_data(a)
    histogram.append(clember.close)
    histogram = [h for h in histogram if h.hist > 1]
    histogram.sort(key=lambda cm: cm.hist, reverse=True)
    clemberlist = histogram[:bins] if len(histogram) > bins else histogram
    for clember in clemberlist:
        clember.data = []
    for data in x:
        dif = float('inf')
        fitcm = None
        for clember in clemberlist:
            dif2 = diff(data, clember, key)
            if dif > dif2:
                dif = dif2
                fitcm = clember
        fitcm.regist_data(data)
    result = [c.data for c in clemberlist if len(c.data) > 0]
    return result


def diff(x, clember, key):
    k = [a - m for a, m in list(zip(x, clember.mean))]
    return key(k)


class Clember:
    def __init__(self, index):
        self.index = index
        self.data, self.mean, self.var, self.hist = [], 0., 0., 0

    def regist_data(self, x):
        self.data.append(x)
        return self

    @property
    def close(self):
        self.hist = len(self.data)
        self.mean = [sum(x) / len(x) for x in list(zip(*self.data))]
        self.var = []
        for x, m in list(zip(list(zip(*self.data)), self.mean)):
            self.var.append(sum([(a - m) ** 2 for a in x]) / self.hist if self.hist > 0 else float('inf'))
        return self
