__author__ = 'leesuk kim'
"""
Radar signal Categorizer
version 2.0
Designed by leesuk kim(aka. LK)
Lang.: Python
Lang. ver.: 3.4.3

"""

from functools import reduce
import math
import time
# import threading

import numpy as np
from scipy.spatial import distance
from scipy.stats import beta
import scipy.stats as scistats


class CPON:
    """
    Class Probability Output Network

    It has classes for classification.
    It has classifiers-SVM, kNN, and CPON.
    """

    def __init__(self):
        self.timestamp = '%d' % int(time.time())  # timestamp
        self.cslist = []  # list of class space
        self.kernel = 1
        pass

    def fit(self, data, target):
        """
        fit
        """
        fcs = self.factory_cs
        set_tag = set(target)

        for tag in set_tag:
            if type(tag) != 'str':
                targetdata = [x[1] for x in list(zip(target, data)) if tag == x[0]]
            else:
                targetdata = [x[1] for x in list(zip(target, data)) if tag in x[0]]
            self.cslist.append(fcs(tag, targetdata))

        for cs in self.cslist:
            cs.clustering()
        pass

    def predict(self, target):
        """predict
        """

        pass

    def predict_ovr(self):
        """predict One vs Rest
        """
        pass

    def factory_cs(self, name: None, data):
        """
        class space factory
        setting kernel volume from this class
        """
        return Class(name, data, self.kernel)


class Class:

    def __init__(self, name, data, kernel: 1):
        self.name = name
        self.data = np.array(data)
        self.data_len = len(data)
        self.dimension = np.array(list(zip(*data)))
        self.dimension_len = len(data[0])
        self.dimension_std = [s.std(ddof=1) for s in self.dimension]
        self.kernel = kernel
        self._centroidlist = []

    def clustering(self):
        # TODO Cluster추가하는 부분
        # clusters = MyCluster.clusters(self.data)
        # 1개면 단순히 전체지만 2개 이상이면 쪼개지겠지
        # feature의 histogram을 그린 후 pdf를 그려서 도합수가 0이 되는 좌표값을 쓰는 건 어떄?
        # histogram은 (max-min)/10의 크기로 쪼갠 10개로 하고.
        # cluster = [[[np.mean(x) for x in self.dimension], self.dimension]]  # kernel = 1 이건 numpy로 계산
        cluster = [[[x.mean() for x in self.dimension], self.data]]  # kernel = 1

        for clst in cluster:
            es = Centroid(self.name, self, clst[0], clst[1])
            self._centroidlist.append(es)
        pass


class Centroid:
    def __init__(self, name: '', class_space: Class, centroid: list, data: list):
        self.name = name
        self.parent = class_space
        self.centroid = centroid
        self.data = data
        self.dimension = np.array(list(zip(*data)))
        self.weight = 1.
        self._dimension_m, self._dimension_s = [d.mean() for d in self.dimension], [d.std(ddof=1) for d in self.dimension]
        self._brv, self._brv_min, self._brv_max = self.__transform_beta__()
        self._brv_m, self.brv_v = np.mean(self._brv), np.var(self._brv, ddof=1)
        self._brv_d, self.brv_p = self.__hypothesis_test__()

    def __hypothesis_test__(self):
        centroid = self.centroid
        brv, brv_m, brv_v = self._brv, self._brv_m, self.brv_v

        ba = brv_m ** 2 * ((1 - brv_m) / brv_v - 1 / brv_m)
        bb = ba * (1 - brv_m) / brv_m
        ba, bb, floc, fscale = beta.fit(brv, ba, bb)
        bcdf = beta.cdf(brv, ba, bb)

        d, pval = scistats.kstest(brv, lambda cdf: bcdf)
        return d, pval

    def __transform_beta__(self):
        brv = [reduce(lambda x, y: x + y, self.gaussian_feature(x)) for x in self.data]  # GKF
        # brv = [self.gaussian_data(x) for x in self.data]  # MGF

        brv, brv_n, brv_x = featurescaling(brv)
        brv.sort()

        return brv, brv_n, brv_x

    def gaussian_data(self, a):
        """
        gaussian function a.k.a. gaussian kernel function
        """
        alpha = self.weight
        amv = list(zip(a, self._dimension_m, self.parent.dimension_std))
        dsquare = 0.5 * reduce(lambda p, q: p + q, [((x - m) ** 2) / ((alpha * s) ** 2) for x, m, s in amv])

        gk = math.exp(-1 * dsquare)
        return gk

    def gaussian_feature(self, f):
        """
        This Gaussian Kernel, exactly, is Multi-dimensional Gaussian Function
        """
        w = self.weight
        kn = [math.exp(-1 * (((x - m) ** 2) / (2 * (w * s) ** 2))) for m, s, x in zip(self._dimension_m, self._dimension_s, f)]
        return kn

    def discriminant(self):
        pass

    def __get_beta__(self):
        # TODO 이게 찾는 거임.
        b = 0
        return b


class PPPC:
    """
    Posterior Probability Parameters of Class
    """
    def __init__(self, pval, d, Y, sw, ba, bb, ecdf, bcdf):
        self._pval = pval
        self._d = d
        self._Y = Y
        self._ymin = min(Y)
        self._ymax = max(Y)
        self._sw = sw  # sigma weight
        self._betaA = ba
        self._betaB = bb
        self._ecdf = ecdf
        self._betaCDF = bcdf

    def mapy2beta(self, y):
        """
        mapping y to beta

        parameters
        ----------
        self: class object
            hypothesis criteria

        y: array_like
            Input array

        returns
        -------
        pair of beta CDF: Float
            Returns a float.
        """
        a = beta.cdf(y, self._betaA, self._betaB)
        if y < self._ymin:
            return 0
        elif y > self._ymax:
            return 1
        else:
            for i, z in enumerate(self._Y):
                if y < z:
                    i = self._Y.index(z)
                    break
        return a
        # return self._betaCDF[i]
    pass


def featurescaling(a, an, ax):
    if type(a) == list:
        an, ax = min(a), max(a)
        return [(x - an) / (ax - an) for x in a], an, ax
    else:
        return (a - an) / (ax - an)