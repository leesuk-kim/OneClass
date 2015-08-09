# -*- coding: cp949 -*-

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
import operator

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
        set_tag = set(target)

        for tag in set_tag:
            targetdata = [x[1] for x in list(zip(target, data)) if tag == x[0]]  # TODO target이 문자열을 때 추가
            self.cslist.append(fcs(tag, targetdata))
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
        return ClassSpace(name, data, self.kernel)


class ClassSpace:

    def __init__(self, name, data, kernel: 1):
        self.name = name
        self.data = data
        self.lendata = len(data)
        self.kernel = kernel


class KernelSpace:
    def __init__(self, name: '', centroid, data):
        self._name = name
        self._centroid = centroid
        self._data = np.array(data)  # subject data
        datatr = np.array(list(zip(*data)))
        self._mean, self._var = [m.mean() for m in datatr], [v.var(ddof=1) for v in datatr]
        self._lendim = len(datatr)
