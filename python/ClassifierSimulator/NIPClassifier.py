__author__ = 'leesuk kim'
"""
Radar signal Categorizer
version 2.0
Designed by leesuk kim(aka. LK)
Lang.: Python
Lang. ver.: 3.4.3

"""
import math
import time
# import threading

import numpy as np
from scipy.stats import beta
import scipy.stats as scistats


class KernelCore:
    def __init__(self, data):
        self._data = data
        self.data_trf = list(zip(*data))
        self.datalen = len(data)
        self.dimlen = len(self.data_trf)
        self.data_mean = [sum(x) / self.datalen for x in self.data_trf]
        self.data_var = [sum([(a - m) ** 2 for a in x]) / self.datalen for x, m in zip(self.data_trf, self.data_mean)]
        self.data_std = [(sum([(a - m) ** 2 for a in x]) / self.datalen) ** .5 for x, m in zip(self.data_trf, self.data_mean)]

    def mahalanobis(self, x):
        """mahalanobis distance"""
        _ned = sum([((_a - _m) ** 2) / _v for _a, _m, _v in zip(x, self.data_mean, self.data_var)])
        _d_m = _ned * .5
        return _d_m


class CPON:
    """
    Class Probability Output Network
    """
    def __init__(self, **kwargs):
        self.timestamp = kwargs['timestamp'] if 'timestamp' in kwargs else '%d' % int(time.time())  # timestamp
        self.cluster = kwargs['cluster'] if 'cluster' in kwargs else 'basic'
        self._verbose = kwargs['verbose'] if 'verbose' in kwargs else False
        self.kernel = 1  # 일단 수동으로 설정합니다.
        self.classes = []  # list of class space
        self.predicts = []
        pass

    def fit(self, data, target):
        """fit"""
        targets = list(set(target))
        self.classes = []
        for _target in targets:
            if type(_target) != 'str':
                data_pos = [x[1] for x in list(zip(target, data)) if _target == x[0]]
                data_neg = [x[1] for x in list(zip(target, data)) if _target != x[0]]
            else:
                data_pos = [x[1] for x in list(zip(target, data)) if _target in x[0]]
                data_neg = [x[1] for x in list(zip(target, data)) if _target not in x[0]]
            self.classes.append(Class(_target, data_pos, data_neg, self.kernel, self._verbose))

        for cs in self.classes:
            cs.fit()
        pass

    def predict(self, data):
        """predict"""
        self.predicts, pred = [], []
        for x in data:
            dcs = self.decision(x)
            pred.append(dcs)

        return pred

    def decision(self, x):
        """
        test vector에 대한 p-value를 test
        :param x: test vector
        :return: xdict: dictionary of p-value
        """
        xdict = {'data': x, 'ppdict': []}
        for cs in self.classes:
            pval = cs.test(x)
            ppdict = {'target': cs.name, 'p-value': pval}
            xdict['ppdict'].append(ppdict)
        xdict['ppdict'] = sorted(xdict['ppdict'], key=lambda a: a['p-value'], reverse=True)
        xdict['target'] = 'No p-value' if xdict['ppdict'][0]['p-value'] == 0. else xdict['ppdict'][0]['target']

        return xdict


def pdf(y: list):
    prob = 1 / len(y)
    _pdf = {}
    bins = set(y)
    for _bin in bins:
        _pdf[_bin] = prob * y.count(_bin)

    return bins, _pdf


def cdf(pdf_dict: dict):
    keyset = pdf_dict.keys()
    keyset = sorted(keyset)
    for key1, key2 in zip(keyset[:-1], keyset[1:]):
        pdf_dict[key2] += pdf_dict[key1]
    return pdf_dict


def beta_shape_parameter(mean, var, lower, upper):
    """first order beta shape parameter estimation
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


def beta_shape_hypothesis_test(brv, brv_m, brv_v):
    """
    test hypothesis whether the p-value from KS-test is acceptable.
    :param brv:
    :param brv_m:
    :param brv_v:
    :return:
    """
    ba, bb = beta_shape_parameter(brv_m, brv_v, 0, 1)
    # ba, bb, floc, fscale = beta.fit(brv, ba, bb)  # TODO 자꾸 계산이 이상하데...
    bcdf = beta.cdf(brv, ba, bb)

    d, pval = scistats.kstest(brv, lambda cdf: bcdf)
    return d, pval, ba, bb


def formula_featurescaling(x, x_max, x_min):
    return 0 if x_max == x_min or x == x_min else (x_max - x_min) / (x - x_min)


def featurescaling_overbound(x, **kwargs):
    """
    :param data:
    :param an:
    :param ax:
    :return:
    """
    if 'x_min' in kwargs and 'x_max' in kwargs:
        return x if type(x) == str else formula_featurescaling(x, kwargs['x_min'], kwargs['x_max'])
    else:
        x_min, x_max = min(x) * 0.99, max(x) * 1.01
        x_fs = [formula_featurescaling(a, x_min, x_max) for a in x]
        return x_fs, x_min, x_max


class Class(KernelCore):
    def __init__(self, name, data_pos: list, data_neg: list, kernel: 1, verbose=False):
        super().__init__(data_pos)
        self.neg = KernelCore(data_neg)
        self.name, self._data, self.kernel = name, data_pos, kernel
        self.kernels = []
        self._knlen = 0
        self.verbose = verbose

        self.y_mean = 0.
        self.y_var = 0.
        self._bp_a, self._bp_b = 0., 0.
        self.weightbook = []
        self.data_bins, self.data_pdf = [], []
        self.data_cdf = []

    def fit(self):
        self.__clustering__()
        self.__build_beta__()

    def __clustering__(self):
        # TODO Cluster하는 부분
        clustergroup = [self._data]  # kernel = 1
        # clustergroup = self.cluster.cluster_lk()

        for cluster in clustergroup:
            es = Kernel(self.name, self, cluster)
            self.kernels.append(es)
        self._knlen = len(self.kernels)

        self.weightbook.append([1])  # weight for kernel of clusters
        pass

    def product(self, x):
        k = sum([kernel.product(x) for kernel in self.kernels])
        return k

    def __build_beta__(self):
        pval_max, pval = 0., 0.
        for weights in self.weightbook:
            # weights = [gen_kernel_weight_rand(5) for _ in range(len(self._kernel))]
            for kn, w in zip(self.kernels, weights):
                kn.weight = w

            y = [self.product(x) for x in self._data]
            # y, yn, yx = featurescaling_overbound(y)
            _bins, _pdf = pdf(y)
            y_cdf = cdf(_pdf).values()

            y_cdf = sorted(y_cdf)
            brv_d, brv_p, brv_a, brv_b = beta_shape_hypothesis_test(y_cdf, np.mean(y_cdf), np.var(y_cdf, ddof=1))

            if pval_max < brv_p:
                weights_max, pval_max = weights, brv_p
                if brv_p > 0.05:
                    self._bp_a, self._bp_b = brv_a, brv_b
                    self.y_mean, self.y_var = np.mean(y), np.var(y, ddof=1)
                    break
                elif self.verbose:
                    print('[' + self.name + '] ' + repr(weights) + ' <- fail')
            a = 1
        pass

    def test(self, x):
        y = self.product(x)
        b = beta.ppf(y, self._bp_a, self._bp_b)
        return b

    def post_prob(self, x):
        pval_max = 0
        for centroid in self.kernels:
            pval = centroid.discriminant(x)
            if pval_max < pval:
                pval_max = pval

        return pval_max, self.name


def formula_gaussian_weight(d, weight):
    return math.exp(-1 * d * weight)


class Kernel(KernelCore):
    def __init__(self, name: str, cls: Class, data: list):
        super().__init__(data)
        self.name, self.parent = name, cls
        self.weight = 1.

    def product(self, x):
        dsquare = self.mahalanobis(x)
        k = formula_gaussian_weight(dsquare, self.weight)
        return k


def __histogram__(x: list, bins: int):
    x = [a for a in x]
    x.sort()
    xn, xx = x[0], x[-1]
    step = (xx - xn) / bins
    box, i = xn + step, 0
    histo = []
    for a in x:
        while a > box:
            histo.append(i)
            i = 0
            box += step
        i += 1
    histo.append(i)
    steps = []
    box = xn
    for _ in range(bins):
        steps.append(box)
        box += step
    return histo, steps


def kernel_weight(size):
    """
    weight generator for kernel
    :param size: length of range
    :return: weight
    """
    mid = int(size / 2)
    for i in range(size):
        yield 2 ** (i - mid)


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


def formula_gaussian_kernel_power(val, mean, std):
    return 0 if val == mean or std == 0 else ((val - mean) ** 2) / (std ** 2)


def formula_gaussian(d):
    return math.exp(-1 * d)


def formula_rayleigh(vector, var):
    return (vector / var) * math.exp(-1 * ((vector ** 2) / (2 * var)))
