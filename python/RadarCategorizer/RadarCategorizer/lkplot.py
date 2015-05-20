import matplotlib.pyplot as pyp
from scipy.stats import beta
import numpy
import os

def plotPDF(vec_x, figname = 'noname', plotnum = 20) : 
    '''bons-fold PDF plot(histogram)'''
    fig, ax = pyp.subplots(1, 1)
    hist = numpy.histogram(vec_x, bins = [(float(x) + 1.) / plotnum for x in range(plotnum)])
    n, bins, patches = ax.hist(vec_x, bins = [(float(x) + 1.) / plotnum for x in range(plotnum)], facecolor='green', alpha=0.5, rwidth = 0.3)
    fig.suptitle(figname + ' PDF')
    #pyp.show()
    path = os.path.join(os.path.dirname(__file__), 'PDF')
    fn = 'PDF_' + figname + '.png'
    fig.savefig(os.path.join(path, fn))
    pyp.close(fig)
    pass
 
def plotCDF(vec_y, figname = 'noname', bins = 100) : 
    '''bins-fold CDF plot(histogram)'''
    fig, ax = pyp.subplots(1, 1)
    cdf = numpy.cumsum(vec_y)
    u = [x / cdf[-1] for x in cdf]
    
    ax.plot(vec_y, range(len(vec_y)), 'k-')
    ax.set_xlabel(r'$F_U(u)$')
    ax.set_ylabel(r'$F_Y(y)$')
    #n, bins, patches = ax.hist(vec_y, bins, cumulative = True, facecolor='green', alpha=0.5, rwidth = 0.3)
    fig.suptitle(figname + ' CDF')
    #pyp.show()
    path = os.path.join(os.path.dirname(__file__), 'CDF')
    fn = 'CDF_' + figname + '.png'
    fig.savefig(os.path.join(path, fn))
    pyp.close(fig)
    pass

def plotBetaPDF(a, b, dataname = 'noname', bins = 100) : 
    '''beta PDF plot(histogram)'''
    fig, ax = pyp.subplots(1, 1)
    x = numpy.linspace(beta.ppf(1 / bins, a, b), beta.ppf(1 - 1 / bins, a, b), bins)
    rv = beta(a, b)
    fig.suptitle(dataname + ' beta PDF')
    ax.set_title(r'$\alpha = {}, \beta = {}$'.format(a, b))
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
    ax.set_title(r'$\alpha = {}, \beta = {}$'.format(a, b))
    ax.plot(x, rv.cdf(x), 'r--', label = 'beta CDF')
    path = os.path.join(os.path.dirname(__file__), 'betaCDF')
    fn = 'beta_CDF_' + dataname + '.png'
    fig.savefig(os.path.join(path, fn))
    #pyp.show()
    pyp.close(fig)
    pass

def plotKStest(y, y_emp, betacdf, a, b, pvalue, dataname = 'noname', bins = 100) : 
    fig, ax = pyp.subplots(1, 1)

    fig.suptitle(dataname + ' beta CDF')
    ax.set_title(r'$\alpha = {}, \beta = {}, p-value = {}$'.format(a, b, pvalue))
    ax.plot(y, betacdf, 'k-', label = 'hypoCDF')
    ax.plot(y, y_emp, 'r--', label = 'empiCDF')
    ax.legend(loc = 4)
    path = os.path.join(os.path.dirname(__file__), 'KStest')
    fn = 'KStest_' + dataname + '.png'
    fig.savefig(os.path.join(path, fn))
    #pyp.show()
    pyp.close(fig)
    pass

def plotOaR(name, onedstc, restdstc) : 
    '''
    plot One-Against-Rest
    '''
    fig, ax = pyp.subplots(1, 1)
    fig.suptitle(name + ' OAR')

    cnk = []
    for i in range(len(onedstc)) : 
        cnk.append(restdstc[i * 50 : (i + 1) * 50])

    ax.plot([x for x in range(len(onedstc))], onedstc, ' o')
    #for rest in restdstc : 
    #    ax.plot([0], [rest], 'k.')
    for res in cnk : 
        ax.plot([x for x in range(len(onedstc))], res, 'k+')

    ax.set_ylim(-1, max(onedstc) * 1.1)
    ax.set_xlim(-1, len(onedstc))
    ax.set_xticks(numpy.arange(0, len(onedstc), 1))
    
    pyp.grid()
    #pyp.show()
    path = os.path.join(os.path.dirname(__file__), 'OAR')
    fn = 'OAR_' + name + '.png'
    fig.savefig(os.path.join(path, fn))
    pyp.close(fig)
    pass

def plotoar(name, map, idx) : 
    
    fig, ax = pyp.subplots(1, 1)
    fig.suptitle(name + ' OAR')
    xaxis = range(len(map[0]))

    for i, submap in enumerate(map) : 
        klist = zip(*submap)
        for k in klist : 
            ax.plot(xaxis, k, 'k+' if i == idx else 'k.')

    ax.set_ylim(-1, 20.)
    ax.set_xlim(-1, len(xaxis) + 1)
    
    pyp.grid()
    #pyp.show()
    path = os.path.join(os.path.dirname(__file__), 'OAR')
    fn = 'OAR_' + name + '.png'
    fig.savefig(os.path.join(path, fn))
    pyp.close(fig)
    pass