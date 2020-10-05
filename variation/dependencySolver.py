import sympy as sy

def fun(varDict, toSolve, n):
    print("\n\n\n")
    print(varDict)
    print(toSolve)
    newDict ={}

    for key in varDict.keys():
        res = [[]]

        for i in range(n):
            print(str(key) +" : " + str(varDict.get(key)))
            sym = sy.Symbol(str(key))
            val = str(varDict.get(key)[i]) + "-" + str(key)
            res.append(val) #TODO - doesnt work!
        newDict.update({key: res})
    
    print(newDict)

   


x = sy.Symbol('x')
a = sy.Symbol('a')
b = sy.Symbol('b')
lindep = sy.Symbol('lindep')

sol = sy.solve(["3-x"],  (x))
#print(len(sol))
print(sol[x])
#print(sol[x])
#print(sol[lindep])

