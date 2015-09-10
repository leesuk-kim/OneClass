__author__ = 'lk'
import NIPIO
import matplotlib.pyplot as plt


class DIO:
    def __init__(self, name):
        self.name = name
        self.data = []


if __name__ == '__main__':
    data, target = NIPIO.import_data()
    e79, e80 = DIO('E0079B0001'), DIO('E0080B0001')

    for d, t in list(zip(data, target)):
        if e79.name in t:
            e79.data.append(d)
        else:
            e80.data.append(d)

    fig = plt.figure()
    plt.title("Acceleration of %s and %s" % (e79.name, e80.name))
    a = 1
    pass
