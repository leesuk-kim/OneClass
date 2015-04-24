from cponlib import CPONNetworkManager
import multiprocessing

if __name__ == '__main__' : 
    print('Hello World')
    multiprocessing.freeze_support()
    obj = CPONNetworkManager(10, 'bc')
    obj.start()
    obj.getResult()