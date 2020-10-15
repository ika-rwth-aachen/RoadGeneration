import sympy as sy
import datetime as dt

def getVarLists(varDict, n):#returns preprocessed lists to be used in sympy library    
    equationList = []

    #list structure [[[<var1>, <var2>][<stringvar1>, <stringvar2>]], ...] outer layer = n entries, inner layer one entry per var. 

    for i in range(n):
        strings = []
        symbols = []
        for key in varDict.keys():            
            sym = sy.Symbol(str(key))
            varString = str(varDict.get(key)[i]) + "-" +str(key)
            strings.append(varString)
            symbols.append(sym)            
    
        equationList.append([symbols, strings])
   
    return equationList


def solveEQ(outerList, n):
    solutions = []
    for i in range(n):
        sol = sy.solve(outerList[i][1], outerList[i][0])
        solutions.append(sol)
   
    return getDict(solutions)

def getDict(symbolDictList):#returns postprocessed dict from sympy solver that can be used in runner.py
    stringDict = {}

    for i in range(len(symbolDictList)):
        for key in symbolDictList[i].keys():
            stringDict.setdefault(str(key), []).append(symbolDictList[i].get(key))
    return stringDict



