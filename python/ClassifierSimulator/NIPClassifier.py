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
import random
# import threading

import numpy as np
from scipy.stats import beta
import scipy.stats as scistats
import NIPCluster


class CPON:
    """
    Class Probability Output Network

    It has classes for classification.
    It has classifiers-SVM, kNN, and CPON.
    """

    def __init__(self, verbose=False):
        self.timestamp = '%d' % int(time.time())  # timestamp
        self.cslist = []  # list of class space
        self.kernel = 1
        self._predict = []
        self.verbose = verbose
        pass

    def fit(self, data, target):
        """
        fit
        """
        fcs = self.factory_cs
        set_tag = list(set(target))
        self.cslist = []
        for tag in set_tag:
            if type(tag) != 'str':
                targetdata = [x[1] for x in list(zip(target, data)) if tag == x[0]]
            else:
                targetdata = [x[1] for x in list(zip(target, data)) if tag in x[0]]
            self.cslist.append(fcs(tag, targetdata))

        # for cs in self.cslist:
        #     cs.clustering()
        pass

    def predict(self, data):
        """predict
        """
        self._predict = []
        pred = []
        for x in data:
            dcs = self.decision(x)
            pred.append(dcs)

        return pred

    def decision(self, x):
        """
        feature x? ???? Posterior Probability of Class Membership ? ????
        ?? Posterior probability? ?? Class? target? return
        :param x: ?? feature
        :return: ?? class? target
        """
        xdict = {'data': x, 'ppdict': []}
        for cs in self.cslist:
            # pp, tg = cs.post_prob(x)
            # ppdict = {'target': tg, 'post_prob': pp}
            # xdict['ppdict'].append(ppdict)
            pval = cs.test(x)
            ppdict = {'target': cs.name, 'p-value': pval}
            xdict['ppdict'].append(ppdict)

        xdict['ppdict'] = sorted(xdict['ppdict'], key=lambda a: a['post_prob'], reverse=True)
        xdict['target'] = 'No p-value' if xdict['ppdict'][0]['post_prob'] == 0. else xdict['ppdict'][0]['target']

        return xdict

    def factory_cs(self, name: None, data):
        """
        class space factory
        setting kernel volume from this class
        """
        return Class(name, data, self.kernel, self.verbose)


class Class:
    def __init__(self, name, data, kernel: 1, verbose=False):
        self.name, self.data, self.kernel = name, data, kernel
        self.dimension = list(zip(*data))
        self.dimlen = len(self.data[0])
        self._centroidlist = []
        self.verbose = verbose

        self.clustering()
        self.weightfit()
        self.build_beta()

    def clustering(self):
        # TODO Cluster하는 부분
        # clustergroup = [self.data]  # kernel = 1
        clustergroup = NIPCluster.cluster(self.data, key=lambda x: sum(x), bins=2)

        for cluster in clustergroup:
            es = Centroid(self.name, self, [np.mean(x) for x in zip(*cluster)], cluster)
            self._centroidlist.append(es)
        pass

    def __kernel__(self, x):
        k = 0
        for ctrd in self._centroidlist:
            k += ctrd.kernel(x)
        return k

    def build_beta(self):
        brv = [self.kernel(x) for x in self.data]
        pass

    def weightfit(self):
        isfit = False
        pval_max, pval = 0., 0.
        while not isfit:
            weights = [random.randint(0, 7) for _ in range(self.dimlen)]
            for ctrd, w in zip(self._centroidlist, weights):
                ctrd.weight = w

            y = [self.__kernel__(x) for x in self.data]
            """
            지금 어디까지 만들었나면...
            centroid들을 기준으로 kernel을 만들어서 linear combination kernel은 구현햇고
            각 centroid에게 적합한 weight를 찾는 과정이야.
            서로 weight를 부여해서 뽑아낸 output들로 beta dist.를 만들고
             1) 그 beta dist로 cpon
             2) beta fitting한 beta model로 cpon
             하면 됨.
            """
            if pval_max < pval:
                weights_max, pval_max = weights, pval

        pass

    def test(self, x):
        pval = 0.
        return pval

    def post_prob(self, x):
        pval_max = 0
        for centroid in self._centroidlist:
            pval = centroid.discriminant(x)
            if pval_max < pval:
                pval_max = pval

        return pval_max, self.name


class Centroid:
    def __init__(self, name: str, cls: Class, centroid: list, data: list):
        self.name, self.parent, self.centroid, self.verbose = name, cls, centroid, cls.verbose
        self.data = data
        self.dimension = np.array(list(zip(*data)))
        self.dimlen = len(self.dimension)
        self._gauss_multiva_param01 = (2 * math.pi) ** self.dimlen
        self._kernel = self.gaussian_data
        self._dimension_m = [d.mean() for d in self.dimension]
        self._dimension_s = [d.std(ddof=1) for d in self.dimension]
        self.weight = 0.
        self._brv, self._rv_min, self._rv_max = 0., 0., 0.
        self._brv_m, self._brv_v = 0., 0.
        self._brv_d, self._brv_p, self._brv_alpha, self._brv_beta = 0., 0., 0., 0.
        self.__centralize__()
        # self._brv_d is Kolmogorov-Smirnov Statistic

    def __centralize__(self):
        # for weight in kernel_weight(1):
        for weight in kernel_weight(7):
            brv, rv_min, rv_max = self.__kernelize__(weight)
            brv_m, brv_v = np.mean(brv), np.var(brv, ddof=1)
            brv_d, brv_p, brv_a, brv_b = beta_shape_hypothesis_test(brv, brv_m, brv_v)

            if brv_p > self._brv_p:
                self.weight = weight
                self._brv, self._rv_min, self._rv_max = brv, rv_min, rv_max
                self._brv_m, self._brv_v = brv_m, brv_v
                self._brv_d, self._brv_p, self._brv_alpha, self._brv_beta = brv_d, brv_p, brv_a, brv_b
            pass
        pass

    def kernel(self, x):
        d = 0
        for a, ca, cs in zip(x, self._dimension_m, self._dimension_s):
            d += ((a - ca) ** 2) / (cs ** 2)
        k = math.exp(-1 * self.weight * d)
        return k

    def __kernelize__(self, weight):
        brv = [self._kernel(x, weight) for x in self.data]  # MGF

        brv, brv_n, brv_x = featurescaling(brv)
        brv.sort()

        return brv, brv_n, brv_x

    def betamapping(self, a):
        q = self._kernel(a, self.weight)  # MGF

        fq = featurescaling(q, self._rv_min, self._rv_max)

        return fq

    def gaussian_data(self, a, w):
        """
        gaussian function a.k.a. gaussian kernel function
        """
        ams = list(zip(a, self._dimension_m, self._dimension_s))
        # linear combination of kernel
        dsquare = -1 * w * reduce(lambda p, q: p + q, [formula_gaussian_kernel_power(x, m, s) for x, m, s in ams])
        gk = math.exp(dsquare)

        return gk

    def discriminant(self, x):
        # TODO transform x to kernel value then KS-test on beta dist.
        beta_x = self.betamapping(x)
        if type(beta_x) == str:
            return 0
        beta_y = beta.ppf(beta_x, self._brv_alpha, self._brv_beta)
        return beta_y


def featurescaling(a, an=0., ax=0.):
    """
    :param a:
    :param an:
    :param ax:
    :return:
    """
    if type(a) == list:
        an, ax = min(a) * 0.99, max(a) * 1.01  # ??? ??? ??? ?? ?? ??
        return [formula_featurescaling(x, an, ax) for x in a], an, ax
    else:
        return a if type(a) == str else formula_featurescaling(a, an, ax)


# formula_gaussian_kernel_power = lambda x, m, s, w: 0 if x == m or w == 0 or s == 0 else ((x - m) ** 2) / (2 * (w * s) ** 2)
formula_gaussian_kernel_power = lambda x, m, s: 0 if x == m or s == 0 else ((x - m) ** 2) / (s ** 2)
formula_featurescaling = lambda p, n, x: 0 if p == n or x == n else (p - n) / (x - n)
formula_norm_euclidean = lambda a: reduce(lambda x, y: x + y ** 2, a) ** 0.5


def beta_shape_parameter(mean, var, lower, upper):
    """
    :param mean:
    :param var:
    :param lower:
    :param upper:
    :return: beta shape parameter alpha and beta
    """
    ml = mean - lower
    um = upper - mean
    ba = (ml / (upper - lower)) * (((ml * um) / var) - 1)
    bb = ba * (um / ml)
    return ba, bb


def kernel_weight(size):
    """
    weight generator for kernel
    :param size: length of range
    :return: weight
    """
    mid = int(size / 2)
    for i in range(size):
        yield 2 ** (i - mid)


def beta_shape_hypothesis_test(brv, brv_m, brv_v):
    """
    test hypothesis whether the p-value from KS-test is acceptable.
    :param brv:
    :param brv_m:
    :param brv_v:
    :return:
    """
    ba, bb = beta_shape_parameter(brv_m, brv_v, 0, 1)
    ba, bb, floc, fscale = beta.fit(brv, ba, bb)
    bcdf = beta.cdf(brv, ba, bb)

    d, pval = scistats.kstest(brv, lambda cdf: bcdf)
    return d, pval, ba, bb


def cov(x, m):
    """
    covariance matrix with a and m
    :param x:
    :param m:
    :return:
    """
    c = []
    for a, ma in list(zip(x, m)):
        row = []
        for b, mb in list(zip(x, m)):
            row.append((a - ma) * (b - mb))
        c.append(row)
    return c
