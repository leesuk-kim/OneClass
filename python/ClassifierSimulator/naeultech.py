"""

Y M - 나 을 텍 전 용 메 인

"""

__author__ = 'lk'

from NIPClassifier import BetaFunction
import NIPIO
from math import *


def ls_logxp1dlog2(x):
    return log(x + 1, 2)


if __name__ == '__main__':
    """
    자 보자...
    일단 우리가 사용하는 데이터는 classifier output이야. 그러니까
    1. Y에 대한 normalize을 해야한다.
    2. beta approximation을 한다.
    3. kstest로 결과 확인.
    """
    data, target = NIPIO.import_data()

    clf_outputs = {}

    # Searching을 통해서 최적의 beta function을 찾아라.
    # a = 1/100을 시작으로 \alpha값을 먼저 최적화하고
    # b = 1/100을 시작으로 \beta값을 먼저 최적화한다.
    # \alpha += a부터 시작하고, p값이 다시 감소한다면 a *= 1/10을 하여 다시 최적화한다.
    # \beta도 alpha와 같은 방법으로 찾는다.

    for d, t in zip(data, target):
        d = d[0]
        # d = ls_logxp1dlog2(d)
        if t not in clf_outputs:
            print(t, 'import start')
            clf_outputs[t] = [d]
        else:
            clf_outputs[t].append(d)

    ckey = "E0001"
    bf = BetaFunction(ckey, None, {'bse': 'mm'})
    bf.aprx_betashape(clf_outputs[ckey])
    print("{}:".format(ckey), bf['p-value'], "var:", bf['std'])

    for ckey in clf_outputs:
        bf = BetaFunction(ckey, None, {'bse': 'mm'})
        bf.aprx_betashape(clf_outputs[ckey])
        # bf.aprx_betashape(clf_outputs[ckey][:100])
        print("{}:".format(ckey), bf['p-value'], "var:", bf['std'])
    print("End py")
