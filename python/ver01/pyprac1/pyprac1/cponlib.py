import numpy as np
import multiprocessing
import lkmodule

#hello comment
TRTS = 2 #hello
#anybody here?

class CPONNetworkManager() : 
    '''
    <members>
    networkName: the name of training data
    foldSize: preferred fold size
    clsSize: recognized class size
    dimSize: dimension of the training data
    centroidSize: recommended centroid size
    discriminant:classification result
    procQueue: multiprocess queue
    procList: multiprocess array
    ***************
    '''
    def __init__(self, ctrdsize, networkname = 'data', clssize = 2, foldsize = 10) : 
        print 'CPONNetworkManager init...target : ' + networkname
        self.networkName = networkname
        self.centroidSize = ctrdsize
        self.clsSize = clssize
        self.foldSize = foldsize
        
        with open('data\\' + self.networkName + '\\train_0_0', 'r') as f : 
            l = f.readline()
            self.dimSize = len(l.split())

        self.procQueue = multiprocessing.Queue()
        self.procList = [CPONFoldManager(i, clssize, self.centroidSize, self.procQueue, networkname) for i in range(foldsize)]
            
        pass            

    def getDimSize(self) : 
        return self.dimSize

    def getNetworkName(self) : 
        return self.networkName

    def getCentroidSize(self) : 
        return self.centroidSize

    def getResult(self) :
        '''
        get probabilistic classification output
        '''
        for proc in self.procList : 
            proc.join()
            print "RESULT: %s" % self.procQueue.get()
        pass

    def start(self) : 
        '''
        start multiprocess
        '''
        for proc in self.procList : 
            proc.start()
        pass

    pass


class CPONFoldManager(multiprocessing.Process) : 
    '''
    <members>
    fold:
    cls:
    ctrd: 
    networkManager:
    networkName:
    clsdata:
    inputs: 
    pcResult: result of probabilistic classification (0-0, 0-1, 1-0, 1-1) for each data
    ***************
    '''
    def __init__(self, fold, cls, ctrd, procqueue, parentName) : 
        super(CPONFoldManager, self).__init__()
        self.queue = procqueue
        self.idx = self.fold = fold
        self.cls = cls
        self.ctrd = ctrd
        ##BAD ACCESS : CANNOT ACCESS PARENT INSTANCE - NO GURRENTY FOR SAME PROCESS
        #self.networkManager = networkManager
        self.networkName = parentName
        #every class has many input manager, and its size is cls x 2(train, test)
        self.inputs = [CPONInputManager(j, self) for j in range(cls)]
        
        ####CHECK DATA####
        pass

    def return_name(self) : 
        ## NOTE: self.name is an attribute of multiprocessing.Process
        return "Process idx=%s is called '%s'" % (self.idx, self.name)

    def run(self) : 
        '''
        return processing result - for this program, it returns classification result.
        '''
        #clustering
        #training
        #
        self.queue.put("asd")
        

    def trainOneClass(self) : 
        '''
        '''
        pass

    def getLda(self) : 
        '''
        '''
        pass
    
    def getDataName(self) : 
        return self.networkName

    def getFold(self) : 
        return self.fold

    def getPcResult(self) : 
        return self.getPcResult
    
    pass


class CPONInputManager :
    '''
    manage inputs foreach fold
    <members>
    networkManager:the class who has this class
    networkName:
    fold:
    cls:   
    traindata:
    testdata:
    ***************
    '''
    def __init__(self, c, fm = CPONFoldManager) : 
        self.networkName = fm.getDataName()
        self.f = fm.getFold()
        self.c = c
        
        #uprolling target datas on the memory
        self.rawData = [self.readData(i, c) for i in range(TRTS)]
        self.datasize = [self.readDataSize(i, c) for i in range(TRTS)]
                        
        ####CHECK DATA
        #print self.dataSize
        #print self.traindata
        #print self.testdata
        #print self.networkManager
        #print self.networkName
        pass

    def readData(self, datatype, cls_c) : 
        #print 'file open : ' + self.getDataPath(CPONInputManager.DATATYPE.reverse_mapping[datatype], cls_c)
        with open(self.getDataPath(CPONInputManager.DATATYPE.reverse_mapping[datatype], cls_c)) as f : 
            buffer = []
            for l in f.readlines() : 
                buffer.append(np.array(l.split(), dtype='f'))

        return np.matrix(buffer, dtype = 'f')
        

    #get data path for open data file
    def getDataPath(self, datatype, cls_c) :
        return 'data\\%s\\%s_%d_%d'% (self.networkName, datatype, self.f, cls_c)

    def readDataSize(self, datatype, cls_c) : 
        '''
        Get data size from target data file
        '''
        with open(self.getDataPath(datatype), 'r') as f : 
            size = len(f.readlines())
        return size
        
    DATATYPE_TRAIN  = 0
    DATATYPE_TEST   = 1
    DATATYPE = lkmodule.enum('train', 'test')
    pass


class CPONCentroidManager : 
    '''
    <members>
    inputManager:
    id:
    ***************
    '''
    def __init__(self, employer, id) : 
        self.inputManager = employer
        self.id = id
        self.candidate = 0
        self.cponRange = [CPONrange() for i in range(5)]
        pass

    pass

#parameters for cpon
class CPONrange : 
    def __init__(self) : 
        pass
    pass