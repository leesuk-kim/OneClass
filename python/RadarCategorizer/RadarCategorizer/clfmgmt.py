# -*- coding: cp949 -*-
"""
Manage Learning Module
"""

from collections import namedtuple
from cppy import cpon
from sklearn.svm import SVC
from sklearn.neighbors import KNeighborsClassifier

class clfmgr : 
    """
    Design Purpose
    --------------
    1. fold learning resource
    2. manage learning modules
    3. manage test result of modules
    히끅 한글 된당...ㅠㅠ
    """
    fold = 10

    def __init__(self, fold = 10) : 
        self.clflist = []
        self.fold = fold
        self.X, self.y = None, None

    def addclf(self, clfitem) : 
        """add classifier module passing by object type check.
        """
        if isinstance(clfitem, moditem) : 
            self.clflist.append(clfitem)
        else : 
            print('please input moditem')
        pass

    def uploadres(self, X, y) :
        """upload learning resources.
        각 클래스별로 업로드된 데이터를 fold할 때 일정하게 나눠지도록 블랜딩한다.
        Parameters
        ----------
        X : {array-like, sparce matrix}, shape = [n_samples, n_features]
            For kernel="precomputed", the expected shape of X is
            [n_samples_test, n_samples_train]

        y : array-like, shape (n_samples,)
            Target values (class labels in classification, real numbers in
            regression)
        Returns
        _______
        self : object
            Returns self.
        """
        lenX, leny = len(X), len(y)
        fold = self.fold

        if lenX != leny : 
            print('Length of X and y are different.')
            return 0
        elif lenX % self.fold != 0 : 
            print('Length of data is not compitible with fold. Modulus is dropped.')

        foldX, foldy = [[] for x in range(fold)], [[] for y in range(fold)]

        for i, zxy in enumerate(list(zip(X, y))) : 
            ii = i % fold
            foldX[ii].append(zxy[0])
            foldy[ii].append(zxy[1])
            pass

        self._foldX, self._foldy = foldX, foldy
        self.X, self.y = X, y
        return self

    def folding(self) : 
        """블랜딩한 데이터를 fold length만큼 나누고, 매 턴마다 해당 fold는 테스트자료, 나머진 학습자료로 정리해서 yield한다.
        """
        foldX, foldy = self._foldX, self._foldy
        fX, fy = [], []
        for j in range(self.fold) : 
            for i in range(self.fold) : 
                if i == j : 
                    pX, py = foldX[i], foldy[i]
                else : 
                    fX.extend(foldX[i])
                    fy.extend(foldy[i])
            
            self.fX, self.fy = fX, fy
            self.pX, self.py = pX, py
            yield fX, fy

    def fit(self, clf) : 
        """fit data and target for classification
        If did not call uploadres() at least once, you fill parameters 'X'(learning resources) and 'y'(learning target)
        이건 그냥 fit하는건데, list로 저장되는게 clf길래 함 만들어봄.
        """
        return clf.fit(self.fX, self.fy)

    def predict(self, clf, X) : 
        y_pred = None
        """Perform classification on samples in X.
        For an one-class model, +1 or -1 is returned.
        이건 그냥 predict하는건데, list로 저장되는게 clf길래 함 만들어봄.
        Parameters
        ----------
        X : {array-like, sparse matrix}, shape = [n_samples, n_features]
            For kernel="precomputed", the expected shape of X is
            [n_samples_test, n_samples_train]
        Returns
        -------
        y_pred : array, shape = [n_samples]
            Class labels for samples in X.
        """
        return clf.predict(self.pX)

    pass

class moditem : 
    """Module item with user-defined name
    """
    def __init__(self, mod=None, modname=None) : 
        self.mod, self.modname = mod, modname
    pass
        
def clffactory(clfname) : 
    """generate classifier by name.
    Options are setted on general.
    Parameters
    ----------
    clfname : string
        The name of clssifier
    Returns
    -------
    mi : moditem object
        
    """
    clfname = clfname.lower()
    clf = None

    if ('svm' in clfname) or ('svc' in clfname): 
        clf = SVC(kernel='rbf', gamma=50)
        print('[clf factory]clf=\'SVC\', kernel=\''+clf.kernel+'\', gamma=\''+repr(clf.gamma)+'\'')
    elif 'knn' in clfname : 
        clf = KNeighborsClassifier(weights='distance')
        print('[clf factory]clf=\'kNN\', weights=\''+clf.weights+'\'')
    elif 'cpon' in clfname : 
        clf = cpon()
        print('[clf factory]clf=\'CPON\'')
    mi = moditem(clf, clfname)
    return mi