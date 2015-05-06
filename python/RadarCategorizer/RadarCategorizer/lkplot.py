import matplotlib.pyplot as pyp
from scipy.stats import beta
import numpy
import os

def plotPDF(data, dataname = 'noname', bins = 20) : 
    '''bons-fold PDF plot(histogram)'''
    fig, ax = pyp.subplots(1, 1)
    hist = numpy.histogram(data, bins = [(float(x) + 1.) / bins for x in range(bins)])
    n, bins, patches = ax.hist(data, bins = [(float(x) + 1.) / bins for x in range(bins)], facecolor='green', alpha=0.5, rwidth = 0.3)
    fig.suptitle(dataname + ' PDF')
    #pyp.show()
    path = os.path.join(os.path.dirname(__file__), 'PDF')
    fn = 'PDF_' + dataname + '.png'
    fig.savefig(os.path.join(path, fn))
    pyp.close(fig)
    pass
 
def plotCDF(data, dataname = 'noname', bins = 100) : 
    '''bins-fold CDF plot(histogram)'''
    fig, ax = pyp.subplots(1, 1)
    n, bins, patches = ax.hist(data, bins, cumulative = True, facecolor='green', alpha=0.5, rwidth = 0.3)
    fig.suptitle(dataname + ' CDF')
    ax.set_ylim([0, 100])
    #pyp.show()
    path = os.path.join(os.path.dirname(__file__), 'CDF')
    fn = 'CDF_' + dataname + '.png'
    fig.savefig(os.path.join(path, fn))
    pyp.close(fig)
    pass

def plotBetaPDF(a, b, dataname = 'noname', bins = 100) : 
    '''beta PDF plot(histogram)'''
    fig, ax = pyp.subplots(1, 1)
    x = numpy.linspace(beta.ppf(1 / bins, a, b), beta.ppf(1 - 1 / bins, a, b), bins)
    rv = beta(a, b)
    fig.suptitle(dataname + ' beta PDF')
    ax.set_title('alpha = %lf, beta = %lf' % (a, b))
    ax.plot(x, rv.pdf(x), 'k-', label='beta PDF')
    path = os.path.join(os.path.dirname(__file__), 'betaPDF')
    fn = 'beta_PDF_' + dataname + '.png'
    fig.savefig(os.path.join(path, fn))
    #pyp.show()
    pyp.close(fig)
    pass

def plotBetaCDF(a, b, dataname = 'noname', bins = 100) : 
    '''beta PDF plot(linear graph)'''
    fig, ax = pyp.subplots(1, 1)
    x = numpy.linspace(beta.ppf(1 / bins, a, b), beta.ppf(1 - 1 / bins, a, b), bins)
    rv = beta(a, b)
    fig.suptitle(dataname + ' beta CDF')
    ax.set_title('alpha = %lf, beta = %lf' % (a, b))
    ax.plot(x, rv.cdf(x), 'r--', label = 'beta CDF')
    path = os.path.join(os.path.dirname(__file__), 'betaCDF')
    fn = 'beta_CDF_' + dataname + '.png'
    fig.savefig(os.path.join(path, fn))
    #pyp.show()
    pyp.close(fig)
    pass