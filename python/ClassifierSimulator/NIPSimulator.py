"""
Manage Learning simulorule
"""
__author__ = 'lk'

from NIPClassifier import CPON
from sklearn.svm import SVC
from sklearn.neighbors import KNeighborsClassifier
from sklearn import metrics
import numpy as np
import copy


default_fold = 2


class ClfSim:
    """
    Design Purpose
    --------------
    1. fold learning resource
    2. manage learning simulorules
    3. manage test result of simulorules
    """

    def __init__(self, fold=2):
        self.simtaglist = []  # list of classifier
        self._fold = fold  # size of fold
        self._data, self._target = None, None
        self._fold_data, self._fold_target = None, None
        self._learn_data, self._learn_target = None, None
        self._pred_data, self._pred_target = None, None

    def addclf(self, simtag):
        """add classifier simulorule with passing by object type check.
        """
        if isinstance(simtag, SimTag):
            self.simtaglist.append(simtag)
        else:
            print('please input SimTag')
        pass

    def fit(self, data, target):
        """upload learning resources.
        ? ????? ???? ???? fold? ? ???? ????? ?????.
        Parameters
        ----------
        X: {array-like, sparce matrix}, shape = [n_samples, n_features]
            For kernel="precomputed", the expected shape of X is
            [n_samples_test, n_samples_train]

        y: array-like, shape (n_samples,)
            Target values (class labels in classification, real numbers in
            regression)
        Returns
        _______
        self: object
            Returns self.
        """
        lendata, lentarget = len(data), len(target)

        f = self._fold

        if lendata != lentarget:
            print('Length of X and y are different.')
            return 0
        elif lendata % self._fold != 0:
            print('Length of data is not compitible with fold. modulus is dropped.')

        fold_data, fold_target = [[] for _ in range(f)], [[] for _ in range(f)]

        for i, zxy in enumerate(list(zip(data, target))):
            ii = i % f
            fold_data[ii].append(zxy[0])
            fold_target[ii].append(zxy[1])

        self._fold_data, self._fold_target = fold_data, fold_target
        self._data, self._target = data, target

        return self

    def folding(self):
        fold_data, fold_target, f = self._fold_data, self._fold_target, self._fold

        for j in range(f):
            ldata, ltarget = [], []  # learning data
            pdata, ptarget = None, None  # predicting data
            for i in range(f):
                if i == j:
                    pdata, ptarget = fold_data[i], fold_target[i]
                else:
                    ldata.extend(fold_data[i])
                    ltarget.extend(fold_target[i])

            self._learn_data, self._learn_target = ldata, ltarget
            self._pred_data, self._pred_target = pdata, ptarget

            yield ldata, ltarget, pdata, ptarget

    def learn(self):
        f = 1
        for fld, flt, fpd, fpt in self.folding():
            for simtag in self.simtaglist:
                simtag.learn(fld, flt, fpd, fpt)
            print("fold%02d complete" % f)
            f += 1

        for simtag in self.simtaglist:
            ave_stats(simtag)

        return self.simtaglist


class SimTag:
    def __init__(self, simulor: None, simulorname: None):
        self.simulor, self.simulorname = simulor, simulorname
        self.predlist = []
        self.statistics = {}
        self.pval_list = []
        self.testtarget = []

    def learn(self, fit_data, fit_target, pred_data, pred_target):
        """

        :param fit_data: data for fit
        :param fit_target: target for fit
        :param pred_data: data for predict
        :param pred_target: target for messurement
        :return:self
        fit, predict and measure statistics on each fold
        """
        self.simulor.fit(fit_data, fit_target)
        pred = self.simulor.predict(pred_data)
        stats = self.statistics
        self.testtarget.append(pred_target)

        # if type(self.simulor) == CPON:
        #     self.simulor.pred_pval

        self.predlist.append(pred)
        # TODO measurement regist
        update_stats(stats, 'acc', metrics.accuracy_score(pred_target, pred))
        # update_stats(stats, 'p_a', metrics.precision_score(pred_target, pred, average='macro'))
        # update_stats(stats, 'p_i', metrics.precision_score(pred_target, pred, average='micro'))
        # update_stats(stats, 'r_a', metrics.recall_score(pred, pred_target, average='macro'))  # NOT COMPITIBLE FOR MULTI-CLASS CLASSIFICATION)
        # update_stats(stats, 'r_i', metrics.recall_score(pred, pred_target, average='micro'))  # NOT COMPITIBLE FOR MULTI-CLASS CLASSIFICATION)
        # update_stats(stats, 'f_a', metrics.f1_score(pred, pred_target, average='macro'))   # NOT COMPITIBLE FOR MULTI-CLASS CLASSIFICATION
        # update_stats(stats, 'f_i', metrics.f1_score(pred, pred_target, average='micro'))   # NOT COMPITIBLE FOR MULTI-CLASS CLASSIFICATION
        update_stats(stats, 'rec', metrics.recall_score(pred, pred_target, average='binary', pos_label=pred_target[0]))  # binary
        update_stats(stats, 'pre', metrics.precision_score(pred_target, pred, average='binary', pos_label=pred_target[0]))  # binary
        update_stats(stats, 'f1m', metrics.f1_score(pred, pred_target, average='binary', pos_label=pred_target[0]))   # bianry
        update_stats(stats, 'scores', confusion_matrix(pred_target, pred))
        update_stats(stats, 'emr', emr_score(pred_target, pred))
        return self

    pass


def clffactory(clfname, **kwargs):
    """
    generate classifier by name.
    Options are setted on general.
    Parameters
    ----------
    clfname: string
        The name of clssifier
    Returns
    -------
    mi: SimTag object

    """
    clfname = clfname.lower()
    clf = None

    if ('svm' in clfname) or ('svc' in clfname):
        k = kwargs['kernel'] if 'kernel' in kwargs else 'rbf'
        g = kwargs['gamma'] if 'gamma' in kwargs else 50
        clf = SVC(kernel=k, gamma=g)
        print("[clf factory]clf=\'SVC\', kernel=\'" + clf.kernel + "\', gamma=\'" + repr(clf.gamma) + "\'")
    elif 'knn' in clfname:
        w = kwargs['weights'] if 'weights' in kwargs else 'distance'
        clf = KNeighborsClassifier(weights=w)
        print("[clf factory]clf=\'kNN\', weights=\'" + clf.weights + "\'")
    elif 'cpon' in clfname:
        c = kwargs['cluster'] if 'cluster' in kwargs else 'lk'
        s = kwargs['bse'] if 'betashape' in kwargs else 'mm'
        b = kwargs['beta'] if 'beta' in kwargs else 'scipy'
        k = kwargs['kernel'] if 'kernel' in kwargs else 'gaussian'
        t = kwargs['threadable'] if 'threadable' in kwargs else False
        clf = CPON(cluster=c, beta=b, bse=s, kernel=k, threadable=t)
        print(clf)
    mi = SimTag(clf, clfname)

    return mi


def ave_stats(simulator_tag: SimTag):
    """
    ? ???? ??? ????, SimTag? SimTag.statistics? ? ?? ???? ????.
    calculate mean of each statistic and append at the end of lsit of statistic.
    :type simulator_tag: SimTag
    :param simulator_tag: object SimTag
    :return: average of statistics
    """
    for key, struct in simulator_tag.statistics.items():
        struct['average'] = np.mean(struct['fold'])

    return simulator_tag


form_stats = {'fold': [], 'average': 0.0}  # data structure for statistic measurement


def update_stats(pedia: dict, key, value):
    """
    ?? ???? fold ? ?????. ???? ??? ?? ??? ? fold? ?????.
    :param pedia: statistics dictionary of classification
    :param key: abbrivation of measurement
    :param value: measurement function
    :return wiki: statistics dictionary of classification
    """
    if key not in pedia:
        pedia[key] = copy.deepcopy(form_stats)

    pedia[key]['fold'].append(value)

    return pedia


def emr_score(target, pred):
    total = len(target)
    tp = 0
    for t, p in list(zip(target, pred)):
        if p == t:
            tp += 1
    emr = tp / total
    return emr


def confusion_matrix(target, pred):
    """
    confusion matrix
    :param target:
    :param pred:
    :return:
    """
    cm_dict = [ConfusionMatrix(x) for x in set(target)]

    for t, p in zip(target, pred):
        if t == p:
            for cm in cm_dict:
                if cm.name == p:
                    cm['tp'] += 1
                else:
                    cm['tn'] += 1
        else:
            for cm in cm_dict:
                if cm.name == p:
                    cm['fp'] += 1
                else:
                    cm['fn'] += 1
    return cm_dict


def dict_keygen(d):
    for i in d:
        yield i


class ConfusionMatrix():
    def __init__(self, name):
        self.name = name
        self.matrix = {'tp': 0, 'tn': 0, 'fp': 0, 'fn': 0}

    def __setitem__(self, key, value):
        self.matrix[key] = value

    def __getitem__(self, item):
        return self.matrix[item]

    def measure(self):
        return self.accuracy(), self.precision(), self.recall(), self.f1measure(), self.exactmatch()

    def accuracy(self):
        if 'accuracy' in self:
            acc = (self['tp'] + self['fn']) / (self['tp'] + self['tn'] + self['fp'] + self['fn'])
            self['accuracy'] = acc
        return self['accuracy']

    def precision(self):
        if 'precision' in self:
            pre = self['tp'] / (self['tp'] + self['fp'])
            self['precision'] = pre
        return self['precision']

    def recall(self):
        if 'recall' in self:
            rec = self['tp'] / (self['tp'] + self['fn'])
            self['recall'] = rec
        return self['recall']

    def f1measure(self):
        if 'f1measure' in self:
            f1m = (2 * self['tp']) / (2 * self['tp'] + self['fp'] + self['fn'])
            self['f1measure'] = f1m
        return self['f1measure']

    def exactmatch(self):
        pass
