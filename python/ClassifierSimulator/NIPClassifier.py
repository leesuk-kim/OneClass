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
        set_tag = list(set(target))

        for tag in set_tag:
            if type(tag) != 'str':
                targetdata = [x[1] for x in list(zip(target, data)) if tag == x[0]]
            else:
                targetdata = [x[1] for x in list(zip(target, data)) if tag in x[0]]
            self.cslist.append(fcs(tag, targetdata))

        for cs in self.cslist:
            cs.clustering()
        pass

    def predict(self, data):
        """predict
        """
        pred = []
        for x in data:
            dcs = self.decision(x)
            pred.append(dcs)

        return pred

    def decision(self, x):
        """
        feature x를 입력하면 Posterior Probability of Class Membership 을 비교해서
        가장 Posterior probability가 낮은 Class의 target을 return
        :param x: 찾을 feature
        :return: 찾은 class의 target
        """
        pp_max, tg_max = 0, 3
        for cs in self.cslist:
            pp, tg = cs.post_prob(x)
            if pp_max < pp:
                pp_max, tg_max = pp, tg
        return tg_max

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

    def post_prob(self, x):
        centroidlist = self._centroidlist
        pval_max = 0
        for centroid in centroidlist:
            pval = centroid.discriminant(x)
            if pval_max < pval:
                pval_max = pval

        return pval_max, self.name


class Centroid:
    def __init__(self, name: '', class_space: Class, centroid: list, data: list):
        self.name = name
        self.parent = class_space
        self.centroid = centroid
        self.data = data
        self.dimension = np.array(list(zip(*data)))
        self.weight = 1.
        self._dimension_m, self._dimension_s = [d.mean() for d in self.dimension], [d.std(ddof=1) for d in self.dimension]
        self._brv, self._data_min, self._data_max = self.__empirical_beta__()
        self._brv_m, self.brv_v = np.mean(self._brv), np.var(self._brv, ddof=1)
        self._brv_d, self._brv_p, self._brv_alpha, self._brv_beta = self.__hypothesis_test__()
        # self._brv_d is Kolmogorov-Smirnov Statistic

    def __hypothesis_test__(self):
        centroid = self.centroid
        brv, brv_m, brv_v = self._brv, self._brv_m, self.brv_v
        # brv_min, brv_max = min(brv), max(brv)
        # brv_min *= 0.9
        # brv_max *= 1.1

        ba, bb = beta_parameter(brv_m, brv_v, 0, 1)
        ba, bb, floc, fscale = beta.fit(brv, ba, bb)
        bcdf = beta.cdf(brv, ba, bb)

        d, pval = scistats.kstest(brv, lambda cdf: bcdf)
        return d, pval, ba, bb

    def __empirical_beta__(self):
        # brv = [reduce(lambda x, y: x + y, self.gaussian_feature(x)) for x in self.data]  # GKF
        brv = [self.gaussian_data(x) for x in self.data]  # MGF

        brv, brv_n, brv_x = featurescaling(brv)
        brv.sort()

        return brv, brv_n, brv_x

    def betamapping(self, a):
        # q = reduce(lambda x, y: x + y, self.gaussian_feature(a))  # GKF
        q = self.gaussian_data(a)  # MGF

        fq = featurescaling(q, self._data_min, self._data_max)

        return fq

    def gaussian_data(self, a):
        """
        gaussian function a.k.a. gaussian kernel function
        """
        w = self.weight
        ams = list(zip(a, self._dimension_m, self.parent.dimension_std))
        # TODO combination of kernel하는 곳입니다.
        # linear combination of kernel
        dsquare = -1 * 0.5 * reduce(lambda p, q: p + q, [gaussian_kernel_power_formula(x, m, s, w) for x, m, s in ams])
        gk = math.exp(dsquare)

        return gk

    def gaussian_feature(self, a):
        """
        This Gaussian Kernel, exactly, is Multi-dimensional Gaussian Function
        """
        w = self.weight
        ams = list(zip(a, self._dimension_m, self._dimension_s))
        kn = [math.exp(gaussian_kernel_power_formula_vertex(x, m, s, w)) for x, m, s in ams]
        return kn

    def discriminant(self, x):
        # TODO x값에 해당하는 f_y를 찾고 이에 대응하는 Beta값을 찾아서 리턴
        beta_x = self.betamapping(x)
        beta_y = beta.ppf(beta_x, self._brv_alpha, self._brv_beta)
        return beta_y


def featurescaling(a, an=0, ax=0):ㄴ
    """
    Min값과 Max값을 return한다는데, 사실은 원래 Min보다 작고 원래 Max보다 큼
    :param a:
    :param an:
    :param ax:
    :return:
    """
    if type(a) == list:
        an, ax = min(a) * 0.99, max(a) * 1.01  # 해주면 적어도 이상한 값은 뜨지 않음
        return [featurescaling_formula(x, an, ax) for x in a], an, ax
    else:
        return featurescaling_formula(a, an, ax)


gaussian_kernel_power_formula_vertex = lambda x, m, s, w: 0 if x == m else -1 * (((x - m) ** 2) / (2 * (s * w) ** 2))
gaussian_kernel_power_formula = lambda x, m, s, w: 0 if x == m else ((x - m) ** 2) / ((w * s) ** 2)
featurescaling_formula = lambda p, n, x: 0 if p == n else (p - n) / (x - n)
euclidean_norm = lambda a: reduce(lambda x, y: x + y ** 2, a) ** 0.5


def beta_parameter(mean, var, lower, upper):
    ml = mean - lower
    um = upper - mean
    ba = (ml / (upper - lower)) * (((ml * um) / var) - 1)
    bb = ba * (um / ml)
    return ba, bb

