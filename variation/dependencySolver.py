import sympy as sy

def fun(varDict, n):
    print("\n\n\n")
    print(varDict)
    #print(toSolve)
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
    print("\n\nLIST:")
    print(equationList)
    solveEQ(equationList, n)


def solveEQ(outerList, n):
    solutions = []
    print("\n\nsolution vectors----------------------")
    for i in range(n):
        print("\n\nsolving:")
        sol = sy.solve(outerList[i][1], outerList[i][0])
        solutions.append(sol)
        print(sol)


x = sy.Symbol('x')
a = sy.Symbol('a')
b = sy.Symbol('b')
lindep = sy.Symbol('lindep')

sol = sy.solve(["3-x"],  (x))
#print(len(sol))
print(sol[x])
#print(sol[x])
#print(sol[lindep])

