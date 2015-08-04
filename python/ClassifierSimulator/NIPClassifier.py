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
        self._timestamp = '%d' % int(time.time())  # timestamp
        self._cslist = []  # list of class space
        self.__kernel = 1
        pass

    def setkernel(self, kn):
        self.__kernel = kn

    def fit(self, data, target):
        """
        fit
        """
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
        cs = ClassSpace(name, data, self.__kernel)

    def regist_cs(self, cs):
        self._cslist.append(cs)


class ClassSpace:

    def __init__(self, name, data, kernel: 1):
        self._name = name
        self._data = data
        self._lendata = len(data)
        self._kernel = kernel


class ClassKernel:
    def __init__(self, name: '', centroid, data):
        self._name = name
        self._centroid = centroid
        self._data = np.array(data)  # subject data
        datatr = np.array(list(zip(*data)))
        self._mean, self._var = [m.mean() for m in datatr], [v.var(ddof=1) for v in datatr]
        self._lendim = len(datatr)
