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
        if 'cpon' in self.simulorname:
            # update_stats(stats, 'emr', emr_score(pred_target, pred))
            pred_dict = pred
            self.pval_list.append(pred_dict)
            pred = [x['target'] for x in pred]

        self.predlist.append(pred)
        # TODO measurement regist
        update_stats(stats, 'acc', metrics.accuracy_score(pred_target, pred))
        update_stats(stats, 'p_a', metrics.precision_score(pred_target, pred, average='macro'))
        update_stats(stats, 'p_i', metrics.precision_score(pred_target, pred, average='micro'))
        update_stats(stats, 'r_a', metrics.recall_score(pred, pred_target, average='macro'))  # NOT COMPITIBLE FOR MULTI-CLASS CLASSIFICATION)
        update_stats(stats, 'r_i', metrics.recall_score(pred, pred_target, average='micro'))  # NOT COMPITIBLE FOR MULTI-CLASS CLASSIFICATION)
        update_stats(stats, 'f_a', metrics.f1_score(pred, pred_target, average='macro'))   # NOT COMPITIBLE FOR MULTI-CLASS CLASSIFICATION
        update_stats(stats, 'f_i', metrics.f1_score(pred, pred_target, average='micro'))   # NOT COMPITIBLE FOR MULTI-CLASS CLASSIFICATION
        update_stats(stats, 'ham', metrics.hamming_loss(pred_target, pred))
        # TODO EMR한글을 살려라...
        update_stats(stats, 'emr', emr_score(pred_target, pred))
        return self

    pass


def clffactory(clfname):
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
        clf = SVC(kernel='rbf', gamma=50)
        print("[clf factory]clf=\'SVC\', kernel=\'"+clf.kernel+"\', gamma=\'"+repr(clf.gamma)+"\'")
    elif 'knn' in clfname:
        clf = KNeighborsClassifier(weights='distance')
        print("[clf factory]clf=\'kNN\', weights=\'"+clf.weights+"\'")
    elif 'cpon' in clfname:
        clf = CPON()
        print("[clf factory]clf='CPON\'")
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


def update_stats(wiki: dict, key, value):
    """
    ?? ???? fold ? ?????. ???? ??? ?? ??? ? fold? ?????.
    :param wiki: statistics dictionary of classification
    :param key: abbrivation of measurement
    :param value: measurement function
    :return wiki: statistics dictionary of classification
    """
    if key not in wiki:
        wiki[key] = copy.deepcopy(form_stats)

    wiki[key]['fold'].append(value)

    return wiki


def emr_score(target, pred):
    total = len(target)
    tp = 0
    for t, p in list(zip(target, pred)):
        if p == t:
            tp += 1
    emr = tp / total
    return emr
