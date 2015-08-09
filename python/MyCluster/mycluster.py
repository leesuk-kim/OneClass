# -*- coding:cp949 -*-
from collections import deque
import numpy
from functools import reduce

__author__ = 'lk'
steer_p, steer_n, steer_a = 'POS', 'NEG', 'AMB'


class MyCluster:
    def __init__(self, features: list):
        self.data = features
        self.dimlen = len(features[0])
        self.dimsteer = {}
        self.dimsteer.update({1: self.dim_steer(features)})

    def dim_steer(self, feature, index=1):
        data = feature
        datalen_half = int(len(data) / 2)
        prev_drct, curr_drct = steer_n, steer_p
        drct_arr = [steer_p]
        subdata = []

        for i in range(self.dimlen - 1):
            if len(data[0]) > 1:
                data = sorted(data, key=lambda x: x[0])
                subdata = [x[i + 1:] for x in data]
                firstdim = [x[0] for x in subdata]

            m_rear = numpy.std(firstdim[datalen_half:], ddof=1)
            m_front = numpy.std(firstdim[:datalen_half], ddof=1)
            m_diff = m_rear - m_front
            m_m = (m_rear + m_front) / 2
            s_total = numpy.std(firstdim, ddof=1)
            s_oneten = 0.9545 * s_total  # 2sigma

            if s_oneten < m_m and index != self.dimlen:
                # TODO 모든 조합을 다 돌아야 합니다. 즉 dim=4라면 4!인 24개.
                # TODO 그러니까 조합 계산기를 만들자!
                index_ = index + 1
                curr_drct = steer_a
                feature_ = deque(list(zip(*feature)))
                feature_.rotate(-1)
                feature_ = list(zip(*feature_))
                self.dimsteer.update({index_: self.dim_steer(feature_, index_)})

            curr_drct = steer_a if s_oneten < m_m else steer_p if m_diff > 0 else steer_n
            drct_arr.append(curr_drct)
            data = subdata
            prev_drct = curr_drct
        # 일단 침착하게 정렬을 하는거야,
        steerset = {}
        for i, drct in enumerate(drct_arr):
            steerset[i] = drct
        return steerset

    def cluster(self):

        return 1


def featurescaling(data):
    fsdata = []
    for dim in list(zip(*data)):
        dn, dx = min(dim), max(dim)
        dd = dx - dn
        fsdata.append([(x - dn) / dd for x in dim])

    return list(zip(*fsdata))
