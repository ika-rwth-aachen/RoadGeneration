import sympy as sy

def getVarLists(varDict, n):
    """This function returns a list containing lists that can be used for the solve method

    Parameters
    ----------
    varDict : dict
        a dict that contains all variables that need to be generated for all n runs
    n : int
        the number of different output files

    Returns
    -------
    list
        a list that can be used in the solve function
    
    """
    #list structure [[[<var1>, <var2>][<stringvar1>, <stringvar2>]], ...] outer layer = n entries, inner layer one entry per var. 

    equationList = []
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
    """This method solves a linear equation system to calculate linar dependent values

    Parameters
    ----------
    outerlist: list
        this is the list that gets generated by getVarList
    n: int
        number of different output files

    Returns
    -------
    dict
        A dict containing all values of the generated variables.
    """
    solutions = []
    for i in range(n):
        sol = sy.solve(outerList[i][1], outerList[i][0])
        solutions.append(sol)
   
    return getDict(solutions)

def getDict(symbolDictList):
    """Returns postprocessed output from solveEQ as dict

    Parameters
    ----------
    symbolDictList
        list of solutions from solveEQ

    Returns
    -------
    A dict containing all values of the generated variables.
    """
 
    stringDict = {}

    for i in range(len(symbolDictList)):
        for key in symbolDictList[i].keys():
            stringDict.setdefault(str(key), []).append(symbolDictList[i].get(key))
    return stringDict


