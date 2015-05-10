def stdscrdistance(arrx, arry, arrdev) : 
    '''
    each array MUST have same length.
    this method uses standard deviation on feature scaling
    '''
    devMax ,devMin = max(arrdev), min(arrdev)
    devran = devMax - devMin
    fsdev = [(dev - devMin) / devran for dev in arrdev]#arrdev

    d = 0.

    param = []
    for i in range(len(fsdev)) : 
        param.append([arrx[i], arry[i], fsdev[i]])
    
    for (x, y, dev) in param :
        d += dev != 0 and ((x - y) ** 2) / dev or 0

    return d ** 0.5

def getMatchAccuracy(arrx, arry, arrdev) : 
    '''
    each array MUST have same length.
    this method uses standard deviation on feature scaling
    '''
    
    devMax ,devMin = max(arrdev), min(arrdev)
    devran = devMax - devMin
    fsdev = [1 - (dev - devMin) / devran for dev in arrdev]#arrdev#
    
    param = []
    for i in range(len(fsdev)) : 
        param.append([arrx[i], arry[i], arrdev[i], fsdev[i]])

    d = 0.
    for (x, y, dev, fs) in param :
        d += dev != 0 and fs * ((x - y) ** 2) / dev or 0
    
    return d ** 0.5
    
