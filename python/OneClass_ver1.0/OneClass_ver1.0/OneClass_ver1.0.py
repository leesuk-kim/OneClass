from OneClass import OneClass

if __name__ == '__main__' : 
    
    obj = OneClass()
    obj.setTrainCase("10f") #10-fold
    obj.setTrainCase("10fe")#10-fold evaluation

    obj.train()
    obj.test()

    obj.printResult()