from numpy.core.multiarray import arange

__author__ = 'lk'


import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
import numpy as np
from sklearn.neighbors.kde import KernelDensity


class DIO:
    def __init__(self, name, data):
        self.name = name
        self.data = data
        self._data_zip = list(zip(*data))
        self.datasize = len(data)
        self.dimsize = len(data[0])
        self.stats_feature = [statistics(x) for x in self._data_zip]

        pass

    def histogram(self, outputdir):
        num_bins = 99
        for dim, stats in list(zip(self._data_zip, self.stats_feature)):
            # n, bins, patches = plt.hist(dim, num_bins, normed=1, facecolor='green', alpha=0.5)
            # y = mlab.normpdf(bins, stats['mean'], stats['std'])
            plt.plot(np.linspace(min(dim), max(dim), 100), stats['pdf'], 'r--')
            plt.show()
            plt.clf()
        pass

    def accelerator(self, outputdir):
        pass

    def acc_24(self, outputdir):
        pass


statistics = lambda x: {
    'mean': np.mean(x),
    'var': np.var(x, ddof=1),
    'std': np.std(x, ddof=1),
    'pdf': pdf(x)
}


def pdf(data: list):
    # hist, bin = np.histogram(data, bins=50)
    # return hist
    kde = KernelDensity(kernel='gaussian', bandwidth=0.1).fit([[x] for x in data])
    b = [[x] for x in np.linspace(min(data), max(data), 100)]
    a = np.exp(kde.score_samples(b))

    return a
